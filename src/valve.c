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
        state.v1_astate = VALVE_CLOSED;
    else if (bit_is_set(MSW_PIN, M1SW1) && bit_is_clear(MSW_PIN, M1SW2))
        state.v1_astate = VALVE_OPEN;
    else
        state.v1_astate = VALVE_MIDDLE;
    
    if (bit_is_clear(MSW_PIN, M2SW1) && bit_is_set(MSW_PIN, M2SW2))
        state.v2_astate = VALVE_CLOSED;
    else if (bit_is_set(MSW_PIN, M2SW1) && bit_is_clear(MSW_PIN, M2SW2))
        state.v2_astate = VALVE_OPEN;
    else
        state.v2_astate = VALVE_MIDDLE;
}

void v1_setdir(eValveAction dir) {
    switch (dir) {
        case CLOSE:
            AIN1_PORT |= _BV(AIN1);
            AIN2_PORT &= ~_BV(AIN2);
            PWMA_PORT |= _BV(PWMA);
            break;
        case OPEN:
            AIN1_PORT &= ~_BV(AIN1);
            AIN2_PORT |= _BV(AIN2);
            PWMA_PORT |= _BV(PWMA);
            break;
        case BREAK:
            PWMA_PORT &= ~_BV(PWMA);
            AIN1_PORT |= _BV(AIN1);
            AIN2_PORT |= _BV(AIN2);
            _delay_ms(V_SHORT_DELAY);
            break;
        case STOP:
            PWMA_PORT &= ~_BV(PWMA);
            AIN1_PORT &= ~_BV(AIN1);
            AIN2_PORT &= ~_BV(AIN2);
            break;
    }
}

void v2_setdir(eValveAction dir) {
    switch (dir) {
        case CLOSE:
            AIN1_2_PORT |= _BV(AIN1_2);
            AIN2_2_PORT &= ~_BV(AIN2_2);
            break;
        case OPEN:
            AIN1_2_PORT &= ~_BV(AIN1_2);
            AIN2_2_PORT |= _BV(AIN2_2);
            break;
        case BREAK:
            AIN1_2_PORT |= _BV(AIN1_2);
            AIN2_2_PORT |= _BV(AIN2_2);
            _delay_ms(V_SHORT_DELAY);
            break;
        case STOP:
            AIN1_2_PORT &= ~_BV(AIN1_2);
            AIN2_2_PORT &= ~_BV(AIN2_2);
            break;
    }
}

eRetCode v_move(eValveMove move) {
    LOG("Moving valve");
    switch (move) {
        case V1_OPEN:
            LOG("1 to OPEN ");
            if (state.v1_astate == VALVE_OPEN) {
                return ALREADY_POSITIONED;
                LOG("APOS!\r\n");
            }
            else if (state.v1_astate == VALVE_CLOSED) {
                state.v1_astate = VALVE_MIDDLE;
                LOG("from CLOSED... ");
                v1_setdir(OPEN);
                STBY_PORT |= _BV(STBY); // Run motor
                V_RUN_TIMEOUT();
                while (bit_is_set(MSW_PIN, M1SW2) && !timeout_flag); // Wait until SW are hit by motor
                V_STOP_TIMEOUT();
                timeout_flag = false;
                v1_setdir(BREAK);
                state.v1_astate = VALVE_OPEN;
                v1_setdir(STOP);
                STBY_PORT &= ~_BV(STBY); // Go back to STBY
                LOG("done.\r\n");
                // LOG(STR_DONE);
                return MOVED;
            }
            else if (state.v1_astate == VALVE_MIDDLE) {
                LOG("from MIDDLE... ");
                v1_setdir(CLOSE);
                STBY_PORT |= _BV(STBY); // Run motor
                _delay_ms(V_BF_DELAY);
                v1_setdir(BREAK);
                v1_setdir(OPEN);
                V_RUN_TIMEOUT();
                while (bit_is_set(MSW_PIN, M1SW2) && !timeout_flag); // Wait until SW are hit by motor
                V_STOP_TIMEOUT();
                timeout_flag = false;
                v1_setdir(BREAK);                
                state.v1_astate = VALVE_OPEN;
                v1_setdir(STOP);
                STBY_PORT &= ~_BV(STBY); // Go back to STBY
                LOG("done.\r\n");
                return MOVED;
            }
            return ERROR;
            break;
        case V1_CLOSE:
            LOG("1 to CLOSED ");
            if (state.v1_astate == VALVE_CLOSED) {
                return ALREADY_POSITIONED;
                LOG("APOS!\r\n");
            }
            else if (state.v1_astate == VALVE_OPEN) {
                LOG("from OPEN... ");
                state.v1_astate = VALVE_MIDDLE;
                v1_setdir(CLOSE);
                STBY_PORT |= _BV(STBY); // Run motor
                V_RUN_TIMEOUT();
                while (bit_is_set(MSW_PIN, M1SW1) && !timeout_flag); // Wait until SW are hit by motor
                V_STOP_TIMEOUT();
                timeout_flag = false;
                v1_setdir(BREAK);
                state.v1_astate = VALVE_CLOSED;
                v1_setdir(STOP);
                STBY_PORT &= ~_BV(STBY); // Go back to STBY
                LOG("done.\r\n");
                return MOVED;
            }
            else if (state.v1_astate == VALVE_MIDDLE) {
                LOG("from MIDDLE... ");
                v1_setdir(OPEN);
                STBY_PORT |= _BV(STBY); // Run motor
                _delay_ms(V_BF_DELAY);
                v1_setdir(BREAK);
                v1_setdir(CLOSE);
                V_RUN_TIMEOUT();
                while (bit_is_set(MSW_PIN, M1SW1) && !timeout_flag); // Wait until SW are hit by motor
                V_STOP_TIMEOUT();
                timeout_flag = false;
                v1_setdir(BREAK);                
                state.v1_astate = VALVE_CLOSED;
                v1_setdir(STOP);
                STBY_PORT &= ~_BV(STBY); // Go back to STBY
                LOG("done.\r\n");
                return MOVED;
            }
            return ERROR;
            break;
        case V2_OPEN:
            LOG("2 to OPEN ");
            if (state.v2_astate == VALVE_OPEN) {
                return ALREADY_POSITIONED;
                LOG("APOS!\r\n");
            }
            else if (state.v2_astate == VALVE_CLOSED) {
                LOG("from CLOSED... ");
                state.v2_astate = VALVE_MIDDLE;
                v2_setdir(OPEN);
                NSLEEP_PORT |= _BV(NSLEEP); // Run motor
                V_RUN_TIMEOUT();
                while (bit_is_set(MSW_PIN, M2SW2) && !timeout_flag); // Wait until SW are hit by motor
                V_STOP_TIMEOUT();
                timeout_flag = false;
                v2_setdir(BREAK);
                state.v2_astate = VALVE_OPEN;
                v2_setdir(STOP);
                NSLEEP_PORT &= ~_BV(NSLEEP); // Go back to STBY
                LOG("done.\r\n");
                return MOVED;
            }
            else if (state.v2_astate == VALVE_MIDDLE) {
                LOG("from MIDDLE... ");
                v2_setdir(CLOSE);
                NSLEEP_PORT |= _BV(NSLEEP); // Run motor
                _delay_ms(V_BF_DELAY);
                v2_setdir(BREAK);
                v2_setdir(OPEN);
                V_RUN_TIMEOUT();
                while (bit_is_set(MSW_PIN, M2SW2) && !timeout_flag); // Wait until SW are hit by motor
                V_STOP_TIMEOUT();
                timeout_flag = false;
                v2_setdir(BREAK);
                state.v2_astate = VALVE_OPEN;
                v2_setdir(STOP);
                NSLEEP_PORT &= ~_BV(NSLEEP); // Go back to STBY
                LOG("done.\r\n");                
                return MOVED;
            }
            return ERROR;
            break;
        case V2_CLOSE:
            LOG("2 to CLOSED ");
            if (state.v2_astate == VALVE_CLOSED) {
                return ALREADY_POSITIONED;
                LOG("APOS!\r\n");
            }
            else if (state.v2_astate == VALVE_OPEN) {
                state.v2_astate = VALVE_MIDDLE;
                LOG("from OPEN... ");
                v2_setdir(CLOSE);
                NSLEEP_PORT |= _BV(NSLEEP); // Run motor
                V_RUN_TIMEOUT();
                while (bit_is_set(MSW_PIN, M2SW1) && !timeout_flag); // Wait until SW are hit by motor
                V_STOP_TIMEOUT();
                timeout_flag = false;
                v2_setdir(BREAK);
                state.v2_astate = VALVE_CLOSED;
                v2_setdir(STOP);
                NSLEEP_PORT &= ~_BV(NSLEEP); // Go back to STBY
                LOG("done.\r\n");
                return MOVED;
            }
            else if (state.v2_astate == VALVE_MIDDLE) {
                LOG("from MIDDLE... ");
                v2_setdir(OPEN);
                NSLEEP_PORT |= _BV(NSLEEP); // Run motor
                _delay_ms(V_BF_DELAY);
                v2_setdir(BREAK);
                v2_setdir(CLOSE);
                V_RUN_TIMEOUT();
                while (bit_is_set(MSW_PIN, M2SW1) && !timeout_flag); // Wait until SW are hit by motor
                V_STOP_TIMEOUT();
                timeout_flag = false;
                v2_setdir(BREAK);
                state.v2_astate = VALVE_CLOSED;
                v2_setdir(STOP);
                NSLEEP_PORT &= ~_BV(NSLEEP); // Go back to STBY
                LOG("done.\r\n");
                return MOVED;
            }
            return ERROR;
            break;
    }
    return NONE;
}
