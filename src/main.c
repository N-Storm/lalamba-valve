/* 
 * Project: lalamba-valve
 * File:   main.c
 * Author: NStorm
 * Created: 04.04.2020 21:06
 */

/*
 * Valve1 - bypass valve, controlled by TB6612FNG (12V)
 * Valve2 - entry valve, controlled by DRV8833 (5V)
 * INT0 attached to MODE BTN active LOW
 * INT1 attached to AC MAINS presence detector active (no AC) LOW (normally HIGH)
 * REED attached to REED sensor active LOW (normally HIGH)
 * MxSW1 - Closed, MxSW2 - Opened
 * AIN1 - Closing direction, AIN2 - opening direction
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/cpufunc.h>
#include <util/delay.h>

#include "main.h"
#include "ws2812_config.h"
#include "light_ws2812.h"
#include "timers.h"

// Globals
volatile state_t state;
settings_t settings;

void inline init() {
    cli();
    // IO settings
    DDRB = _BV(WS2812) | _BV(AIN1_2) | _BV(AIN2_2) | _BV(PWMA);
    DDRC = _BV(AIN1) | _BV(AIN2) | _BV(STBY) | _BV(NSLEEP);
    DDRD = _BV(TX);
    MSW_PORT |= _BV(M1SW1) | _BV(M1SW2) | _BV(M2SW1) | _BV(M2SW2); // Enable pull-ups

    // Ext interrupt settings
    MCUCR = _BV(ISC11) | _BV(ISC01); // Falling edge mode for INT0, INT1
    
    // Timer interrupt settings
    TIMSK = _BV(TOIE0); // Enable T0 overflow interrupts (timer is still disabled)
    
    SET_LED(WHITE); // Turn on white LED
    sei();
}

// Updates actual (based on switches) valve states
void update_valve_astates() {
    if (bit_is_set(MSW_PIN, M1SW1) && bit_is_clear(MSW_PIN, M1SW2))
        state.v1_astate = VALVE_CLOSED;
    else if (bit_is_clear(MSW_PIN, M1SW1) && bit_is_set(MSW_PIN, M1SW2))
        state.v1_astate = VALVE_OPEN;
    else
        state.v1_astate = VALVE_MIDDLE;
    
    if (bit_is_set(MSW_PIN, M2SW1) && bit_is_clear(MSW_PIN, M2SW2))
        state.v2_astate = VALVE_CLOSED;
    else if (bit_is_clear(MSW_PIN, M2SW1) && bit_is_set(MSW_PIN, M2SW2))
        state.v2_astate = VALVE_OPEN;
    else
        state.v2_astate = VALVE_MIDDLE;
}

void v1_setdir(eValveDir dir) {
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

eRetCode v_move(eValveMove move) {
    switch (move) {
        case V1_OPEN:
            if (state.v1_astate == VALVE_OPEN)
                return ALREADY_POSITIONED;
            else if (state.v1_astate == VALVE_CLOSED) {
                state.v1_astate = VALVE_MIDDLE;
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
                return MOVED;
            }
            else if (state.v1_astate == VALVE_MIDDLE) {
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
                return MOVED;
            }
            return ERROR;
            break;
        case V1_CLOSE:
            if (state.v1_astate == VALVE_CLOSED)
                return ALREADY_POSITIONED;
            else if (state.v1_astate == VALVE_OPEN) {
                state.v1_astate = VALVE_MIDDLE;
                v1_setdir(CLOSE);
                STBY_PORT |= _BV(STBY); // Run motor
                V_RUN_TIMEOUT();
                while (bit_is_set(MSW_PIN, M1SW2) && !timeout_flag); // Wait until SW are hit by motor
                V_STOP_TIMEOUT();
                timeout_flag = false;
                v1_setdir(BREAK);
                state.v1_astate = VALVE_CLOSED;
                v1_setdir(STOP);
                STBY_PORT &= ~_BV(STBY); // Go back to STBY
                return MOVED;
            }
            else if (state.v1_astate == VALVE_MIDDLE) {
                v1_setdir(OPEN);
                STBY_PORT |= _BV(STBY); // Run motor
                _delay_ms(V_BF_DELAY);
                v1_setdir(BREAK);
                v1_setdir(CLOSE);
                V_RUN_TIMEOUT();
                while (bit_is_set(MSW_PIN, M1SW2) && !timeout_flag); // Wait until SW are hit by motor
                V_STOP_TIMEOUT();
                timeout_flag = false;
                v1_setdir(BREAK);                
                state.v1_astate = VALVE_CLOSED;
                v1_setdir(STOP);
                return MOVED;
            }
            return ERROR;
            break;
        case V2_OPEN:
            
            break;
        case V2_CLOSE:
            break;
    }
    return NONE;
}

void calibrate() {
    update_valve_astates();

    if (state.v1_astate == VALVE_CLOSED)
        state.v1_sstate = state.v1_astate;
    else if (state.v1_astate == VALVE_OPEN) {
        v_move(V1_CLOSE);
    }

    if (state.v2_astate == VALVE_OPEN)
        state.v2_sstate = state.v2_astate;
    else {
        v_move(V2_OPEN);
    }
    
    SET_LED(GREEN);
}

int main(void)
{
    init();
    calibrate();
    EINT_ENABLE();
    
    while (1) 
    {
    }
}
