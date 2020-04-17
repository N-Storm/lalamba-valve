/*
 * Project: lalamba-valve
 * File:   fsm.c
 * Author: NStorm
 * Created: 12.04.2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <avr/wdt.h>

#include "main.h"
#include "fsm.h"
#include "valve.h"
#include "ws2812_config.h"
#include "light_ws2812.h"

eState fsWaterClosed() {
    if (state.v2_astate == VST_OPEN) {
        v_move(MV_V2_CLOSE);
        EINT_ENABLE();
        SET_LED(VIOLET);
        return ST_WATER_CLOSED;
    } else {
        LOGP(STR_VALVE_POS_ERROR);
        return state.prev_state;
    }
}

eState fsWaterClosedToNormal() {
    if (state.v2_astate == VST_CLOSED) {
        SET_LED(VIOLET);
        v_move(MV_V2_OPEN);
        if (state.v1_astate == VST_OPEN)
            v_move(MV_V1_CLOSE);
        EINT_ENABLE();
        SET_LED(GREEN);
        return ST_NORMAL;
    } else {
        LOGP(STR_VALVE_POS_ERROR);
        return state.prev_state;
    }
}

eState fsBypassToNormal() {
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
    fsWaterClosed();
    SET_LED(RED);
    state.flags.reed = false;
    return ST_REED_OVERFLOW;
}

eState fsToggleBypass() {
    if (state.v1_astate == VST_CLOSED)
        v_move(MV_V1_OPEN);
    else if (state.v1_astate == VST_OPEN)
        v_move(MV_V1_CLOSE);
    else {
        LOGP(STR_VALVE_POS_ERROR);
        return state.prev_state;
    }
    EINT_ENABLE();
    switch (state.prev_state) {
        case ST_NORMAL:
            SET_LED(BLUE);
            return ST_BYPASS;
            break;
        case ST_BYPASS:
            SET_LED(GREEN);
            return ST_NORMAL;
            break;
        default:
            return state.prev_state;
            break;
    }
}

eState fsRestoration() {
    return ST_RESTORATION;
}

eState fsBackFromRestoration() {
    if (state.v2_astate == VST_CLOSED) {
        v_move(MV_V2_OPEN);
        EINT_ENABLE();
        if (state.v1_astate == VST_OPEN) {
            SET_LED(BLUE);
            return ST_BYPASS;
        } else if (state.v1_astate == VST_CLOSED) {
            SET_LED(GREEN);
            return ST_NORMAL;
        } else
            return ST_VALVE_TIMEOUT;
    } else {
        LOGP(STR_VALVE_POS_ERROR);
        return state.prev_state;
    }
}

eState fsMaintenance() {
    return ST_NORMAL;
}

eState fsReset() {
    SET_LED(BLACK);
    wdt_enable(WDTO_250MS);
    while(1);
    calibrate();
    return ST_NORMAL;
}

// Transition table
trans_t trans = { 
    [ST_NORMAL][EV_BTN_SHORT] = fsToggleBypass,
    [ST_NORMAL][EV_BTN_LONG] = fsWaterClosed,
    [ST_NORMAL][EV_BTN_EXTRA_LONG] = fsMaintenance,
    [ST_NORMAL][EV_REED] = fsReed,
    [ST_BYPASS][EV_REED] = fsReed,
    [ST_BYPASS][EV_BTN_SHORT] = fsBypassToNormal,
    [ST_BYPASS][EV_BTN_LONG] = fsWaterClosed,
    [ST_BYPASS][EV_AC_RESTORATION] = fsBypassToNormal,
    [ST_WATER_CLOSED][EV_BTN_SHORT] = fsToggleBypass,
    [ST_WATER_CLOSED][EV_BTN_LONG] = fsWaterClosedToNormal,
    [ST_WATER_CLOSED][EV_BTN_EXTRA_LONG] = fsWaterClosedToNormal,
    [ST_REED_OVERFLOW][EV_REED_RESTORATION] = fsRestoration,
    [ST_REED_OVERFLOW][EV_BTN_SHORT] = fsToggleBypass,
    [ST_RESTORATION][EV_BTN_LONG] = fsBackFromRestoration,
    [ST_VALVE_TIMEOUT][EV_BTN_EXTRA_LONG] = fsReset,
    [ST_ANY][EV_VALVE_TIMEOUT] = fsTimeout
};

// Get event in order of priority
eEvent fsGetEvent() {
    eEvent ret = EV_NONE;

    if (state.flags.reed) {
        state.flags.reed = false;
        ret = EV_REED;
    } else if (state.flags.restoration) {
        state.flags.restoration = false;
        ret = EV_REED_RESTORATION;
    } else if (state.flags.timeout) {
        state.flags.timeout = false;
        ret = EV_VALVE_TIMEOUT;
    } else if (state.flags.ac_restored) {
        state.flags.ac_restored = false;
        ret = EV_AC_RESTORATION;
    } else if (state.btn_state == BTN_SHORT)
        ret = EV_BTN_SHORT;
    else if (state.btn_state == BTN_LONG)
        ret = EV_BTN_LONG;
    else if (state.btn_state == BTN_EXTRA_LONG)
        ret = EV_BTN_EXTRA_LONG;

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