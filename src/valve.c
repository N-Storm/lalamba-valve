/*
 * Project: lalamba-valve
 * File:   valve.c
 * Author: NStorm
 * Created: 10.04.2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/cpufunc.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "main.h"
#include "valve.h"
#include "timers.h"
#include "saveload.h"

// Updates actual (based on switches) valve states
void v_update_states() {
    LOG("Checking valve states.\r\n");
    if (bit_is_clear(MSW_PIN, M1SW1) && bit_is_set(MSW_PIN, M1SW2))
        state.v1_state = VST_CLOSED;
    else if (bit_is_set(MSW_PIN, M1SW1) && bit_is_clear(MSW_PIN, M1SW2))
        state.v1_state = VST_OPEN;
    else
        state.v1_state = VST_MIDDLE;

    if (bit_is_clear(MSW_PIN, M2SW1) && bit_is_set(MSW_PIN, M2SW2))
        state.v2_state = VST_CLOSED;
    else if (bit_is_set(MSW_PIN, M2SW1) && bit_is_clear(MSW_PIN, M2SW2))
        state.v2_state = VST_OPEN;
    else
        state.v2_state = VST_MIDDLE;
}

// Helper function for v_move() to prepare ports before actually running the driver for valve1
void v1_setdir(eValveAction dir) {
    switch (dir) {
        case ACT_CLOSE:
            AIN1_PORT |= _BV(AIN1);
            AIN2_PORT &= ~_BV(AIN2);
            PWMA_PORT |= _BV(PWMA);
            break;
        case ACT_OPEN:
            AIN1_PORT &= ~_BV(AIN1);
            AIN2_PORT |= _BV(AIN2);
            PWMA_PORT |= _BV(PWMA);
            break;
        case ACT_BREAK:
            PWMA_PORT &= ~_BV(PWMA);
            AIN1_PORT |= _BV(AIN1);
            AIN2_PORT |= _BV(AIN2);
            _delay_ms(V_SHORT_DELAY);
            break;
        case ACT_STOP:
            PWMA_PORT &= ~_BV(PWMA);
            AIN1_PORT &= ~_BV(AIN1);
            AIN2_PORT &= ~_BV(AIN2);
            break;
    }
}

// Helper function for v_move() to prepare ports before actually running the driver for valve2
void v2_setdir(eValveAction dir) {
    switch (dir) {
        case ACT_CLOSE:
            AIN1_2_PORT |= _BV(AIN1_2);
            AIN2_2_PORT &= ~_BV(AIN2_2);
            break;
        case ACT_OPEN:
            AIN1_2_PORT &= ~_BV(AIN1_2);
            AIN2_2_PORT |= _BV(AIN2_2);
            break;
        case ACT_BREAK:
            AIN1_2_PORT |= _BV(AIN1_2);
            AIN2_2_PORT |= _BV(AIN2_2);
            _delay_ms(V_SHORT_DELAY);
            break;
        case ACT_STOP:
            AIN1_2_PORT &= ~_BV(AIN1_2);
            AIN2_2_PORT &= ~_BV(AIN2_2);
            break;
    }
}

eRetCode v1_move() {
    return RET_ERROR;
}

eRetCode v_check_pos(eValveMove move) {
    v_update_states();
    switch (move) {
        case MV_V1_OPEN:
            if (state.v1_state == VST_OPEN)
                return RET_ALREADY_POSITIONED;
            break;
        case MV_V1_CLOSE:
            if (state.v1_state == VST_CLOSED)
                return RET_ALREADY_POSITIONED;
            break;
        case MV_V2_OPEN:
            if (state.v2_state == VST_OPEN)
                return RET_ALREADY_POSITIONED;
            break;
        case MV_V2_CLOSE:
            if (state.v2_state == VST_CLOSED)
                return RET_ALREADY_POSITIONED;
            break;
    }
    return RET_OK;
}

void static inline v_log_direction(eValveMove move) {
        LOG("Moving valve ");
        if (move == MV_V1_CLOSE || move == MV_V1_OPEN)
            LOG("1 ");
        else
            LOG("2 ");
        if ((state.v1_state == VST_CLOSED && move == MV_V1_OPEN) || (state.v2_state == VST_CLOSED && move == MV_V2_OPEN))
            LOGP(STR_CLOSED);
        else if ((state.v1_state == VST_OPEN && move == MV_V1_CLOSE) || (state.v2_state == VST_OPEN && move == MV_V2_CLOSE))
            LOGP(STR_OPEN);
        else
            LOGP(STR_MIDDLE);
        LOG("->");
        if (move == MV_V1_OPEN || move == MV_V2_OPEN)
            LOGP(STR_OPEN);
        else
            LOGP(STR_CLOSED);
        LOG(" ");
}

// Function which actually moves valve. TODO: Fix a lot of repeative code.

eRetCode v_move(eValveMove move) {
    eRetCode ret = RET_NONE;
    
    // Check valve position before moving.
    if (v_check_pos(move) == RET_ALREADY_POSITIONED) {
        LOGP(STR_APOS);
        return RET_ALREADY_POSITIONED;
    }

#ifdef LOGS
    v_log_direction(move);
#endif

    EINT_DISABLE();

    switch (move) {
        case MV_V1_OPEN:
            if (state.v1_state == VST_CLOSED) {
                state.v1_state = VST_MIDDLE;
            } else if (state.v1_state == VST_MIDDLE) {
                v1_setdir(ACT_CLOSE);
                STBY_PORT |= _BV(STBY); // Run motor
                _delay_ms(V_BF_DELAY);
                v1_setdir(ACT_BREAK);
            }
            v1_setdir(ACT_OPEN);
            STBY_PORT |= _BV(STBY); // Run motor
            RUN_TIMEOUT(V_ROT_OVF_SIMPLE);
            while (bit_is_set(MSW_PIN, M1SW2) && !t0_timeout_flag); // Wait until SW are hit by motor
            STOP_TIMEOUT();
            v1_setdir(ACT_BREAK);
            v1_setdir(ACT_STOP);
            STBY_PORT &= ~_BV(STBY); // Go back to STBY
            if (t0_timeout_flag) {
                LOGP(STR_TIMEOUT);
                t0_timeout_flag = false;
                state.flags.timeout = true;
                state.v1_state = VST_MIDDLE;
                ret = RET_TIMEOUT;
            } else {
                state.v1_state = VST_OPEN;
                LOGP(STR_DONE);
                ret = RET_MOVED;
            }
            break;
        case MV_V1_CLOSE:
            if (state.v1_state == VST_OPEN) {
                state.v1_state = VST_MIDDLE;
            } else if (state.v1_state == VST_MIDDLE) {
                v1_setdir(ACT_OPEN);
                STBY_PORT |= _BV(STBY); // Run motor
                _delay_ms(V_BF_DELAY);
                v1_setdir(ACT_BREAK);
            }
            v1_setdir(ACT_CLOSE);
            STBY_PORT |= _BV(STBY); // Run motor
            RUN_TIMEOUT(V_ROT_OVF_SIMPLE);
            while (bit_is_set(MSW_PIN, M1SW1) && !t0_timeout_flag); // Wait until SW are hit by motor
            STOP_TIMEOUT();
            v1_setdir(ACT_BREAK);
            v1_setdir(ACT_STOP);
            STBY_PORT &= ~_BV(STBY); // Go back to STBY
            if (t0_timeout_flag) {
                LOGP(STR_TIMEOUT);
                t0_timeout_flag = false;
                state.flags.timeout = true;
                state.v1_state = VST_MIDDLE;
                ret = RET_TIMEOUT;
            } else {
                state.v1_state = VST_CLOSED;
                LOGP(STR_DONE);
                ret = RET_MOVED;
            }
           break;
        case MV_V2_OPEN:
            if (state.v2_state == VST_CLOSED) {
                state.v2_state = VST_MIDDLE;
            } else if (state.v2_state == VST_MIDDLE) {
                v2_setdir(ACT_CLOSE);
                NSLEEP_PORT |= _BV(NSLEEP); // Run motor
                _delay_ms(V_BF_DELAY);
                v2_setdir(ACT_BREAK);
            }
            v2_setdir(ACT_OPEN);
            NSLEEP_PORT |= _BV(NSLEEP); // Run motor
            RUN_TIMEOUT(V_ROT_OVF_SIMPLE);
            while (bit_is_set(MSW_PIN, M2SW2) && !t0_timeout_flag); // Wait until SW are hit by motor
            STOP_TIMEOUT();
            v2_setdir(ACT_BREAK);
            v2_setdir(ACT_STOP);
            NSLEEP_PORT &= ~_BV(NSLEEP); // Go back to STBY
            if (t0_timeout_flag) {
                LOGP(STR_TIMEOUT);
                t0_timeout_flag = false;
                state.flags.timeout = true;
                state.v2_state = VST_MIDDLE;
                ret = RET_TIMEOUT;
            } else {
                state.v2_state = VST_OPEN;
                LOGP(STR_DONE);
                ret = RET_MOVED;
            }
            break;
        case MV_V2_CLOSE:
            if (state.v2_state == VST_OPEN) {
                state.v2_state = VST_MIDDLE;
            } else if (state.v2_state == VST_MIDDLE) {
                v2_setdir(ACT_OPEN);
                NSLEEP_PORT |= _BV(NSLEEP); // Run motor
                _delay_ms(V_BF_DELAY);
                v2_setdir(ACT_BREAK);
            }
            v2_setdir(ACT_CLOSE);
            NSLEEP_PORT |= _BV(NSLEEP); // Run motor
            RUN_TIMEOUT(V_ROT_OVF_SIMPLE);
            while (bit_is_set(MSW_PIN, M2SW1) && !t0_timeout_flag); // Wait until SW are hit by motor
            STOP_TIMEOUT();
            v2_setdir(ACT_BREAK);
            v2_setdir(ACT_STOP);
            NSLEEP_PORT &= ~_BV(NSLEEP); // Go back to STBY
            if (t0_timeout_flag) {
                LOGP(STR_TIMEOUT);
                t0_timeout_flag = false;
                state.flags.timeout = true;
                state.v2_state = VST_MIDDLE;
                ret = RET_TIMEOUT;
            } else {
                state.v2_state = VST_CLOSED;
                LOGP(STR_DONE);
                ret = RET_MOVED;
            }
            break;
    }
    EINT_ENABLE();
    return ret;
}

void v_calibrate() {
    LOG("Calibration begin.\r\n");
    v_update_states();

    if (state.v1_state != VST_CLOSED)
        v_move(MV_V1_CLOSE);

    if (state.v2_state != VST_OPEN)
        v_move(MV_V2_OPEN);

    state.cur_state = ST_NORMAL;
    LOG("Calibration done.\r\n");
}
