/* 
 * Project: lalamba-valve
 * File:   fsm.c
 * Author: NStorm
 * Created: 12.04.2020
 */

#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "fsm.h"
#include "valve.h"
#include "ws2812_config.h"
#include "light_ws2812.h"

eState fsNormalBypass() {
    if (state.v1_astate == VST_CLOSED) {
        v_move(MV_V1_OPEN);
        EINT_ENABLE();
        SET_LED(BLUE);
        return ST_BYPASS;
    } else {
        LOGP(STR_VALVE_POS_ERROR);
        return ST_NORMAL;
    }
}

eState fsNormalWaterClosed() {
    return ST_WATER_CLOSED;
}

eState fsBypassNormal() {
    if (state.v1_astate == VST_OPEN) {
        v_move(MV_V1_CLOSE);
        EINT_ENABLE();
        SET_LED(GREEN);
        return ST_NORMAL;
    } else {
        LOGP(STR_VALVE_POS_ERROR);
        return ST_BYPASS;
    }
}

eState fsTimeout() {
    SET_LED(RED);
    state.flags.timeout = false;
    return ST_VALVE_TIMEOUT;
}

eState fsReed() {
    SET_LED(RED);
    state.flags.reed = false;
    return ST_REED_OVERFLOW;
}

// Transition table
trans_t trans = { 
    [ST_NORMAL][EV_BTN_SHORT] = fsNormalBypass,
    [ST_NORMAL][EV_BTN_LONG] = fsNormalWaterClosed,
    [ST_NORMAL][EV_REED] = fsReed,
    [ST_BYPASS][EV_REED] = fsReed,
    [ST_BYPASS][EV_BTN_SHORT] = fsBypassNormal,
    [ST_ANY][EV_VALVE_TIMEOUT] = fsTimeout
};

// Get event in order of priority
eEvent fsGetEvent() {
    eEvent ret = EV_NONE;
    
    if (state.flags.reed) // FIXME cont running
        ret = EV_REED;
    else if (state.flags.timeout)
        ret = EV_VALVE_TIMEOUT;
    else if (state.flags.ac_shortage)
        ret = EV_AC_SHORTAGE;
    else if (state.btn_state == BTN_SHORT)
        ret = EV_BTN_SHORT;
    else if (state.btn_state == BTN_LONG)
        ret = EV_BTN_LONG;
    
    if (ret == EV_BTN_SHORT || ret == EV_BTN_LONG || ret == EV_BTN_EXTRA_LONG)
        state.btn_state = BTN_NONE;

    return ret;
}

// Run the transition based on event
eRetCode fsTransition() {
#ifdef VERBOSE_LOGS
    LOG("Transition: st %d, ev %d ", state.cur_state, state.event);
#endif
    if ((state.event < EV_ANY) && (trans[state.cur_state][state.event] != NULL || trans[ST_ANY][state.event] != NULL)) {
        state.prev_state = state.cur_state; // save previous state
        if (trans[state.cur_state][state.event] != NULL) {
#ifdef VERBOSE_LOGS
            LOG("found.\r\n");
#endif
            state.cur_state = trans[state.cur_state][state.event](); // Run the transition
        }
        else if (trans[ST_ANY][state.event] != NULL) { // Catch on any state handler
#ifdef VERBOSE_LOGS
            LOG("ST_ANY.\r\n");
#endif
            state.cur_state = trans[ST_ANY][state.event]();
        }
        return RET_OK;
    }
#ifdef VERBOSE_LOGS
    LOG("not found.\r\n");
#endif
    return RET_ERROR;
}