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

// Transition table
trans_t trans = { 
    [ST_NORMAL][EV_BTN_SHORT] = fsNormalBypass,
    [ST_NORMAL][EV_BTN_LONG] = fsNormalWaterClosed,
    [ST_BYPASS][EV_BTN_SHORT] = fsBypassNormal
};

// Get event in order of priority
eEvent fsGetEvent() {
    eEvent ret = EV_NONE;
    
    if (!GET_REED()) // FIXME cont running
        ret = EV_REED;
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
    LOG("Transition: st %d, ev %d\r\n", state.cur_state, state.event);
#endif
    if ((state.event < EV_LAST) && trans[state.cur_state][state.event] != NULL) {
        state.prev_state = state.cur_state; // save previous state
        state.cur_state = trans[state.cur_state][state.event](); // run the transition
        return RET_OK;
    }
    return RET_ERROR;
}