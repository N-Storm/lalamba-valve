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
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "main.h"
#include "ws2812_config.h"
#include "light_ws2812.h"
#include "timers.h"
#include "saveload.h"

// Globals
volatile state_t state;
settings_t settings;

#ifdef LOGS
int uart_putchar(char c, FILE *stream);
FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void USART_Init() {
    /* Set baud rate to 57600 */
    UBRRL = 34;
    UCSRA = 1 << U2X;
    /* Enable receiver and transmitter */
    UCSRB = (1 << RXEN) | (1 << TXEN);
    /* Set frame format: 8N1 */
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

void USART_Transmit(unsigned char data) {
    /* Wait for empty transmit buffer */
    while (!(UCSRA & (1 << UDRE)));
    /* Put data into buffer, sends the data */
    UDR = data;
}

int uart_putchar(char c, FILE *stream) {
    USART_Transmit((unsigned char)c);
    return 0;
}
#endif

void init() {
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
    
#ifdef LOGS
    USART_Init();
    stdout = &mystdout;
#endif
    
    SET_LED(WHITE); // Turn on white LED
    sei();
}

void calibrate() {
    LOG("Calibration begin.\r\n");
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
    LOG("Calibration done.\r\n");
}

int main(void)
{
    init();
    LOG("Init done.\r\n");
    calibrate();
    EINT_ENABLE();
    
    while (1) 
    {
    }
}
