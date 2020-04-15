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
void update_valve_astates() {
    LOG("Checking valve states.\r\n");
    if (bit_is_clear(MSW_PIN, M1SW1) && bit_is_set(MSW_PIN, M1SW2))
        state.v1_astate = VST_CLOSED;
    else if (bit_is_set(MSW_PIN, M1SW1) && bit_is_clear(MSW_PIN, M1SW2))
        state.v1_astate = VST_OPEN;
    else
        state.v1_astate = VST_MIDDLE;

    if (bit_is_clear(MSW_PIN, M2SW1) && bit_is_set(MSW_PIN, M2SW2))
        state.v2_astate = VST_CLOSED;
    else if (bit_is_set(MSW_PIN, M2SW1) && bit_is_clear(MSW_PIN, M2SW2))
        state.v2_astate = VST_OPEN;
    else
        state.v2_astate = VST_MIDDLE;
}

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

eRetCode v_move(eValveMove move) {
    LOG("Moving valve");
    EINT_DISABLE();
    switch (move) {
        case MV_V1_OPEN:
            LOG("1");
            LOGP(STR_TO_OPEN);
            if (state.v1_astate == VST_OPEN) {
                LOGP(STR_APOS);
                return RET_ALREADY_POSITIONED;
            }
            else if (state.v1_astate == VST_CLOSED || state.v1_astate == VST_MIDDLE) {
                if (state.v1_astate == VST_CLOSED) {
                    state.v1_astate = VST_MIDDLE;
                    LOGP(STR_FROM_CLOSED);
                } else if (state.v1_astate == VST_MIDDLE) {
                    LOGP(STR_FROM_MIDDLE);
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
                    state.v1_astate = VST_MIDDLE;
                    return RET_TIMEOUT;
                } else {
                    state.v1_astate = VST_OPEN;
                    LOGP(STR_DONE);
                    return RET_MOVED;
                }
            }
            LOGP(STR_ERROR);
            return RET_ERROR;
            break;
        case MV_V1_CLOSE:
            LOG("1");
            LOGP(STR_TO_CLOSED);
            if (state.v1_astate == VST_CLOSED) {
                LOGP(STR_APOS);
                return RET_ALREADY_POSITIONED;
            }
            else if (state.v1_astate == VST_OPEN || state.v1_astate == VST_MIDDLE) {
                if (state.v1_astate == VST_OPEN) {
                    LOGP(STR_FROM_OPEN);
                    state.v1_astate = VST_MIDDLE;
                } else if (state.v1_astate == VST_MIDDLE) {
                    LOGP(STR_FROM_MIDDLE);
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
                    state.v1_astate = VST_MIDDLE;
                    return RET_TIMEOUT;
                } else {
                    state.v1_astate = VST_CLOSED;
                    LOGP(STR_DONE);
                    return RET_MOVED;
                }
            }
            LOGP(STR_ERROR);
            return RET_ERROR;
            break;
        case MV_V2_OPEN:
            LOG("2");
            LOGP(STR_TO_OPEN);
            if (state.v2_astate == VST_OPEN) {
                LOGP(STR_APOS);
                return RET_ALREADY_POSITIONED;
            }
            else if (state.v2_astate == VST_CLOSED || state.v2_astate == VST_MIDDLE) {
                if (state.v2_astate == VST_CLOSED) {
                    LOGP(STR_FROM_CLOSED);
                    state.v2_astate = VST_MIDDLE;
                } else if (state.v2_astate == VST_MIDDLE) {
                    LOGP(STR_FROM_MIDDLE);
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
                    state.v2_astate = VST_MIDDLE;
                    return RET_TIMEOUT;
                } else {
                    state.v2_astate = VST_OPEN;
                    LOGP(STR_DONE);
                    return RET_MOVED;
                }
            }
            LOGP(STR_ERROR);
            return RET_ERROR;
            break;
        case MV_V2_CLOSE:
            LOG("2");
            LOGP(STR_TO_CLOSED);
            if (state.v2_astate == VST_CLOSED) {
                return RET_ALREADY_POSITIONED;
                LOGP(STR_APOS);
            }
            else if (state.v2_astate == VST_OPEN || state.v2_astate == VST_MIDDLE) {
                if (state.v2_astate == VST_OPEN) {
                    state.v2_astate = VST_MIDDLE;
                    LOGP(STR_FROM_OPEN);
                } else if (state.v2_astate == VST_MIDDLE) {
                    LOGP(STR_FROM_MIDDLE);
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
                    state.v2_astate = VST_MIDDLE;
                    return RET_TIMEOUT;
                } else {
                    state.v2_astate = VST_CLOSED;
                    LOGP(STR_DONE);
                    return RET_MOVED;
                }
            }
            LOGP(STR_ERROR);
            return RET_ERROR;
            break;
    }
    EINT_ENABLE();
    return RET_NONE;
}
