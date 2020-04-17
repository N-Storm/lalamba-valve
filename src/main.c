/*
 * Project: lalamba-valve
 * File:   main.c
 * Author: NStorm
 * Created: 04.04.2020
 */

/*
 * Valve1 - bypass valve, controlled by TB6612FNG (12V)
 * Valve2 - entry valve, controlled by DRV8833 (5V)
 * INT0 attached to MODE BTN active LOW
 * INT1 attached to AC MAINS presence detector active (no AC) HIGH (normally LOW)
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
#include "fsm.h"

// Globals
volatile state_t state;
settings_t settings;
const ptr_t bootloader_start = (ptr_t)((FLASHEND - 511) >> 1);

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
    MCUCR = _BV(ISC10) | _BV(ISC00); // Any logic change in INT0, INT1 cause interrupt

    // Timer interrupt settings
    TIMSK = _BV(TOIE0); // Enable T0 overflow interrupts (timer is still disabled)

#ifdef LOGS
    USART_Init();
    stdout = &mystdout;
#endif

    _delay_ms(1);
    SET_LED(WHITE); // Turn on white LED
    _NOP();

    sei();
}

void calibrate() {
    LOG("Calibration begin.\r\n");
    update_valve_astates();

    if (state.v1_astate == VST_CLOSED)
        state.v1_sstate = state.v1_astate;
    else
        v_move(MV_V1_CLOSE);

    if (state.v2_astate == VST_OPEN)
        state.v2_sstate = state.v2_astate;
    else
        v_move(MV_V2_OPEN);

    state.cur_state = ST_NORMAL;
    SET_LED(GREEN);
    LOG("Calibration done.\r\n");
}

void static inline UART_rx() {
    static char uart_buf[3];

    if (!uart_buf[0])
        uart_buf[0] = UDR;
    else {
        uart_buf[1] = UDR;
        if (uart_buf[0] == 'B' && uart_buf[1] == 'L') {
            cli();
            bootloader_start();
        }
        else {
            uart_buf[0] = 0;
            uart_buf[1] = 0;
        }
    }
}

void static inline led_blink() {
    static uint16_t cnt = 0;

    cnt++;
    if (cnt == 32768) {
        if (state.v1_astate == VST_CLOSED) {
            SET_LED(GREEN);
        } else if (state.v1_astate == VST_OPEN) {
            SET_LED(BLUE);
        }
    } else if (cnt == 0) {
        switch (state.cur_state) {
            case ST_WATER_CLOSED:
                SET_LED(VIOLET);
                break;
            case ST_REED_OVERFLOW:
                SET_LED(RED);
                break;
            case ST_RESTORATION:
                SET_LED(BLACK)
                break;
            default:
                break;
        }
    }
}

int main(void)
{
    init();
    LOG("Init done.\r\n");
#ifdef VERBOSE_LOGS
    LOG("BTN_SHORT_TICKS = %d, BTN_LONG_TICKS = %d.\r\n", BTN_SHORT_TICKS, BTN_LONG_TICKS);
    LOG("BTN_SHORT_OVF = %d, BTN_LONG_OVF = %d.\r\n", BTN_SHORT_OVF, BTN_LONG_OVF);
    LOG("Bootloader start = 0x%X\r\n", bootloader_start);
    LOG("Transition table size = %d, count = %d.\r\n", sizeof(trans), (sizeof(trans)/sizeof(*trans)));
#endif
    calibrate();
    save_settings();
    EINT_ENABLE();

    while(1)
    {
        state.event = fsGetEvent();
        if (state.event != EV_NONE) {
            fsTransition();
            // state.event = EV_NONE; // reset event
        }
        if (!state.flags.reed && !GET_REED() && (state.cur_state == ST_NORMAL || state.cur_state == ST_BYPASS)) // Poll reed sensor
            state.flags.reed = true;
        else if (state.cur_state == ST_REED_OVERFLOW && GET_REED()) // Poll reed sensor
            state.flags.restoration = true;

        if (bit_is_set(UCSRA, RXC)) // Poll UART for incoming bytes;
            UART_rx();

        if (state.cur_state == ST_WATER_CLOSED || state.cur_state == ST_REED_OVERFLOW || state.cur_state == ST_RESTORATION) // Work on LED blinking modes
            led_blink();
    }
}
