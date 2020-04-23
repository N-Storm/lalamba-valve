/*
 * Project: lalamba-valve
 * File:   fsm.c
 * Author: NStorm
 * Created: 12.04.2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "main.h"
#include "fsm.h"
#include "valve.h"
#include "ws2812_config.h"
#include "light_ws2812.h"
#include "saveload.h"
#include "timers.h"

void delay_s(uint8_t s) {
    while (s) {
        _delay_ms(1000);
        s--;        
    }
}

// See transition table below for these function descriptions and when they occur
eState fsError() {
    SET_LED(RED);
    state.flags.error = false;
    return ST_VALVE_ERR;
}

eState fsWaterClosed() {
    if (state.v2_state != VST_CLOSED) {
        SET_LED(VIOLET_HALF);
        v_move(MV_V2_CLOSE);
    } else {
        LOGP(STR_VALVE_POS_ERROR);
        return state.prev_state;
    }
    if (state.v1_state != VST_CLOSED) {
        v_move(MV_V1_CLOSE);
    }
    SET_LED(VIOLET);
    return ST_WATER_CLOSED;
}

eState fsWaterClosedToNormal() {
    if (state.v2_state != VST_OPEN) {
        SET_LED(GREEN_HALF);
        v_move(MV_V2_OPEN);
        if (state.v1_state == VST_OPEN)
            v_move(MV_V1_CLOSE);
        SET_LED(GREEN);
        return ST_NORMAL;
    }
    LOGP(STR_VALVE_POS_ERROR);
    return fsError();
}

 eState fsReed() {
    SET_LED(RED_HALF);
    v_move(MV_V2_CLOSE);
    SET_LED(RED);
    state.flags.reed = false;
    return ST_REED_OVERFLOW;
}

eState fsToggleBypass() {
    if (state.v1_state == VST_CLOSED) {
        SET_LED(BLUE_HALF);
        v_move(MV_V1_OPEN);
    } else if (state.v1_state == VST_OPEN) {
        SET_LED(GREEN_HALF);
        v_move(MV_V1_CLOSE);
    } else {
        LOGP(STR_VALVE_POS_ERROR);
        return fsError();
    }

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

eState fsAcRestoration() {
    state.flags.ac_restored = false;
    
    if (state.v1_state != VST_CLOSED)
        return fsToggleBypass();
    else
        return ST_NORMAL;
}

eState fsRestoration() {
    return ST_RESTORATION;
}

eState fsBackFromRestoration() {
    if (state.v2_state != VST_OPEN) {
        if (state.v1_state == VST_OPEN)
            SET_LED(BLUE_HALF);
        else if (state.v1_state == VST_CLOSED)
            SET_LED(GREEN_HALF);
        v_move(MV_V2_OPEN);
        if (state.v1_state == VST_OPEN) {
            SET_LED(BLUE);
            return ST_BYPASS;
        } else if (state.v1_state == VST_CLOSED) {
            SET_LED(GREEN);
            return ST_NORMAL;
        }
    }
    LOGP(STR_VALVE_POS_ERROR);
    return fsError();
}

eState fsMaintenance() {
    STOP_TIMEOUT();
    SET_LED(YELLOW);
    if (state.v1_state != VST_CLOSED || state.v2_state != VST_OPEN) {
        LOGP(STR_VALVE_POS_ERROR);
        return fsError();
    }
    v_move(MV_V2_CLOSE);
    v_move(MV_V1_OPEN);
    EINT_DISABLE();
    delay_s(30);
    EINT_ENABLE();
    v_move(MV_V1_CLOSE);
    v_move(MV_V2_OPEN);

    SET_LED(GREEN); // Back to normal state
    return ST_NORMAL;
}

// Will set current state to ST_NONE, save settings and reboot MCU via WDT to get re-calibration done on boot
eState fsReset() {
    SET_LED(BLACK);
    state.cur_state = ST_NONE;
    save_settings(SAVE_FULL);
    wdt_enable(WDTO_250MS);
    cli();
    while(1);
}

// Transition table
trans_t trans = { 
    [ST_NORMAL][EV_BTN_SHORT] = fsToggleBypass,
    [ST_NORMAL][EV_BTN_LONG] = fsWaterClosed,
    [ST_NORMAL][EV_BTN_EXTRA_LONG] = fsMaintenance,
    [ST_NORMAL][EV_REED] = fsReed,
    [ST_BYPASS][EV_REED] = fsReed,
    [ST_BYPASS][EV_BTN_SHORT] = fsToggleBypass,
    [ST_BYPASS][EV_BTN_LONG] = fsWaterClosed,
    [ST_BYPASS][EV_AC_RESTORATION] = fsAcRestoration,
    [ST_WATER_CLOSED][EV_BTN_SHORT] = fsToggleBypass,
    [ST_WATER_CLOSED][EV_BTN_LONG] = fsWaterClosedToNormal,
    [ST_WATER_CLOSED][EV_BTN_EXTRA_LONG] = fsWaterClosedToNormal,
    [ST_REED_OVERFLOW][EV_REED_RESTORATION] = fsRestoration,
    [ST_REED_OVERFLOW][EV_BTN_SHORT] = fsToggleBypass,
    [ST_RESTORATION][EV_BTN_LONG] = fsBackFromRestoration,
    [ST_VALVE_ERR][EV_BTN_EXTRA_LONG] = fsReset,
    [ST_ANY][EV_VALVE_TIMEOUT] = fsError
};

// Return event in order of priority by checking flags
eEvent fsGetEvent() {
    eEvent ret = EV_NONE;

    if (state.flags.reed) {
        state.flags.reed = false;
        ret = EV_REED;
    } else if (state.flags.restoration) {
        state.flags.restoration = false;
        ret = EV_REED_RESTORATION;
    } else if (state.flags.error) {
        state.flags.error = false;
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

// Reset btn_state if we're handling EV_BTN* event
    if (ret == EV_BTN_SHORT || ret == EV_BTN_LONG || ret == EV_BTN_EXTRA_LONG)
        state.btn_state = BTN_NONE;

    return ret;
}

// Run the transition based on event
eRetCode fsTransition() {
#ifdef VERBOSE_LOGS
    LOG("Transition: st %d, ev %d ", state.cur_state, state.event);
#endif

    // First check if there is function associated with state/event or for ST_ANY special handler
    if ((state.event < EV_ANY) && (trans[state.cur_state][state.event] != NULL || trans[ST_ANY][state.event] != NULL)) {
        state.prev_state = state.cur_state; // save previous state
        if (trans[state.cur_state][state.event] != NULL) {
#ifdef VERBOSE_LOGS
            LOG("found.\r\n");
#endif
            state.cur_state = trans[state.cur_state][state.event](); // Run the transition
        }
        else if (trans[ST_ANY][state.event] != NULL) { // Catch on any state special handler
#ifdef VERBOSE_LOGS
            LOG("ST_ANY.\r\n");
#endif
            state.cur_state = trans[ST_ANY][state.event](); // Run the transition
        }
        save_settings(SAVE_FULL);
        LOG("Switched to state: %d\r\n", state.cur_state);
        return RET_OK;
    }
#ifdef VERBOSE_LOGS
    LOG("not found.\r\n");
#endif
    return RET_ERROR;
}