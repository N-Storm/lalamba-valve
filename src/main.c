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
 * REED_PIN attached to REED sensor active LOW (normally HIGH)
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

// Array for WS2818 LED colors
struct cRGB leda = WHITE;

void inline init() {
    cli();
    // IO settings
    DDRB = _BV(WS2812_PIN) | _BV(AIN1_2_PIN) | _BV(AIN2_2_PIN) | _BV(PWMA_PIN);
    DDRC = _BV(AIN1_PIN) | _BV(AIN2_PIN) | _BV(STBY_PIN) | _BV(NSLEEP_PIN);
    DDRD = _BV(TX_PIN) | _BV(INT1_PIN) | _BV(M1SW1_PIN) | _BV(M1SW2_PIN) | _BV(M2SW1_PIN) | _BV(M2SW2_PIN);

    // Ext interrupt settings
    MCUCR = (1 << ISC11) | (1 << ISC01); // Falling edge mode for INT0, INT1
    
    ws2812_setleds(&leda, 1); // Turn on white LED
    sei();
}

void inline enable_extint() {
    GICR = (1 << INT1) | (1 << INT0); // Enable INT0, INT1    
}

// Check reed sensor reading. Return true if reed is HIGH (normal).
bool inline check_reed() {
    return bit_is_set(REED_PORT, REED_PIN);
}

// Updates actual (based on switches) valve states
void update_valve_astates() {
    if (bit_is_set(M1SW1_PORT, M1SW1_PIN) && bit_is_clear(M1SW2_PORT, M1SW2_PIN))
        state.v1_astate = VALVE_CLOSED;
    else if (bit_is_clear(M1SW1_PORT, M1SW1_PIN) && bit_is_set(M1SW2_PORT, M1SW2_PIN))
        state.v1_astate = VALVE_OPEN;
    else
        state.v1_astate = VALVE_MIDDLE;
    
    if (bit_is_set(M2SW1_PORT, M2SW1_PIN) && bit_is_clear(M2SW2_PORT, M2SW2_PIN))
        state.v2_astate = VALVE_CLOSED;
    else if (bit_is_clear(M2SW1_PORT, M2SW1_PIN) && bit_is_set(M2SW2_PORT, M2SW2_PIN))
        state.v2_astate = VALVE_OPEN;
    else
        state.v2_astate = VALVE_MIDDLE;
}

void v_move(eValveMove move) {
    switch(move) {
        case V1_CLOSE:
        case V1_OPEN:
            v1_move(move);
            break;
        case V2_CLOSE:
        case V2_OPEN:
            v2_move(move);
            break;
    }

    if (move == V1_CLOSE || move == V1_OPEN)
        v1_move(move);
    else if (move == V2_CLOSE || move == V2_OPEN)
        v2_move(move);
}

void v1_move(eValveMove move) {
    switch(move) {
        case V1_OPEN:
            if (state.v1_astate == VALVE_OPEN)
                return;
            break;
        case V1_CLOSE:
            break;
    }
}

void v2_move(eValveMove move) {
}

void calibrate() {
    update_valve_astates();
    if (state.v1_astate == VALVE_CLOSED)
        state.v1_sstate = state.v1_astate;
    else if (state.v1_astate == VALVE_OPEN) {
        
    }

    if (state.v2_astate == VALVE_OPEN)
        state.v2_sstate = state.v2_astate;
    else {
        
    }
}

int main(void)
{
    init();
    calibrate();
    enable_extint();
    
    while (1) 
    {
    }
}
