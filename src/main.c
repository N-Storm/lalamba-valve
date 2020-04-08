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
// Array for WS2818 LED colors
struct cRGB leda = WHITE;
volatile state_t state;
settings_t settings;

void inline init() {
    cli();
    // IO settings
    DDRB = _BV(WS2812) | _BV(AIN1_2) | _BV(AIN2_2) | _BV(PWMA);
    DDRC = _BV(AIN1) | _BV(AIN2) | _BV(STBY) | _BV(NSLEEP);
    DDRD = _BV(TX);
    MSW_PORT |= (1 << M1SW1) | (1 << M1SW2) | (1 << M2SW1) | (1 << M2SW2); // Enable pull-ups

    // Ext interrupt settings
    MCUCR = (1 << ISC11) | (1 << ISC01); // Falling edge mode for INT0, INT1
    
    ws2812_setleds(&leda, 1); // Turn on white LED
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

eRetCode v_move(eValveMove move) {
    switch(move) {
        case V1_OPEN:
            if (state.v1_astate == VALVE_OPEN)
                return ALREADY_POSITIONED;
            else if (state.v1_astate == VALVE_CLOSED) {
                state.v1_astate = VALVE_MIDDLE;
                AIN1_PORT &= ~_BV(AIN1);
                AIN2_PORT |= _BV(AIN2);
                PWMA_PORT |= _BV(PWMA);
                STBY_PORT |= _BV(STBY); // Run motor!
                // TODO: Add timer0 timeout
                loop_until_bit_is_set(MSW_PIN, M1SW1); // Wait until SW are hit by motor
                PWMA_PORT &= ~(_BV(PWMA)); // Short brake
                _delay_ms(10);
                AIN2_PORT &= _BV(AIN2);
                STBY_PORT &= ~_BV(STBY); // Go back to STBY
                state.v1_astate = VALVE_OPEN;
                return MOVED;
            }
            break;
        case V1_CLOSE:
            if (state.v1_astate == VALVE_CLOSED)
                return ALREADY_POSITIONED;
            else if (state.v1_astate == VALVE_OPEN) {
                state.v1_astate = VALVE_MIDDLE;
                AIN2_PORT &= ~_BV(AIN2);
                AIN1_PORT |= _BV(AIN1);
                PWMA_PORT |= _BV(PWMA);
                STBY_PORT |= _BV(STBY); // Run motor!
                // TODO: Add timer0 timeout
                loop_until_bit_is_set(MSW_PIN, M1SW2); // Wait until SW are hit by motor
                PWMA_PORT &= ~(_BV(PWMA)); // Short brake
                _delay_ms(10);
                AIN1_PORT &= _BV(AIN1);
                STBY_PORT &= ~_BV(STBY); // Go back to STBY
                state.v1_astate = VALVE_CLOSED;
                return MOVED;
            }
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
}

int main(void)
{
    init();
    calibrate();
    EINT_ENABLE();
    testf();
    
    while (1) 
    {
    }
}
