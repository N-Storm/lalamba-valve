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
#include "saveload.h"

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

void calibrate() {
    update_valve_astates();

    if (state.v1_astate == VALVE_CLOSED)
        state.v1_sstate = state.v1_astate;
    else
        v_move(V1_CLOSE);

    if (state.v2_astate == VALVE_OPEN)
        state.v2_sstate = state.v2_astate;
    else
        v_move(V2_OPEN);
    
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
