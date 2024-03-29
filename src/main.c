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
#include <avr/wdt.h>

#include "main.h"
#include "ws2812_config.h"
#include "light_ws2812.h"
#include "timers.h"
#include "saveload.h"
#include "fsm.h"

// Globals
volatile state_t state;
// settings_t settings;
const ptr_t bootloader_start = (ptr_t)((FLASHEND - 511) >> 1);

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

void inline WDT_off(void) {
    /* reset WDT */
    wdt_reset();
    /* Write logical one to WDCE and WDE */
    WDTCR |= (1 << WDCE) | (1 << WDE);
    /* Turn off WDT */
    WDTCR = 0x00;
}

void init() {
    cli();
    WDT_off();
    // IO settings
    DDRB = _BV(WS2812) | _BV(AIN1_2) | _BV(AIN2_2) | _BV(PWMA);
    DDRC = _BV(AIN1) | _BV(AIN2) | _BV(STBY) | _BV(NSLEEP);
    DDRD = _BV(TX);
    MSW_PORT |= _BV(M1SW1) | _BV(M1SW2) | _BV(M2SW1) | _BV(M2SW2); // Enable pull-ups

    // Ext interrupt settings
    MCUCR = _BV(ISC10) | _BV(ISC00); // Any logic change in INT0, INT1 cause interrupt

    // Timer interrupt settings
    TIMSK = _BV(TOIE0); // Enable T0 overflow interrupts (timer is still disabled)
    
    USART_Init();

#ifdef LOGS
    stdout = &mystdout;
#endif

    _delay_ms(1);
    SET_LED(WHITE); // Turn on white LED
    MCUCSR = 0; // Reset power on reason register to avoid tampering with bootloader
    sei();
}

void static inline UART_rx() {
    static char uart_buf[3];

    if (!uart_buf[0])
        uart_buf[0] = UDR;
    else {
        uart_buf[1] = UDR;
        if (uart_buf[0] == 'B' && uart_buf[1] == 'L') {
            cli();
            LOG("BL instruction received, jumping to bootloader\r\n");
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
        if (state.v1_state == VST_CLOSED) {
            SET_LED(GREEN);
        } else if (state.v1_state == VST_OPEN) {
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
                SET_LED(BLACK);
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
    LOG("BTN_SHORT_TICKS = %d, BTN_LONG_TICKS = %d\r\n", BTN_SHORT_TICKS, BTN_LONG_TICKS);
    LOG("BTN_SHORT_OVF = %d, BTN_LONG_OVF = %d\r\n", BTN_SHORT_OVF, BTN_LONG_OVF);
    LOG("Settings size = %d, EEPROM_ENTRIES = %d\r\n", EEPROM_ENTRY_SIZE, EEPROM_ENTRIES);
    LOG("Bootloader start = 0x%X\r\n", bootloader_start);
    LOG("Transition table size = %d, count = %d.\r\n", sizeof(trans), (sizeof(trans)/sizeof(*trans)));
#endif
    load_settings();
    
// Button are held during boot for manual reset. TODO: Check power-on reason.
    if (bit_is_clear(BTN_PIN, BTN)) {
        uint8_t rcnt = 0;
        while(bit_is_clear(BTN_PIN, BTN) && rcnt < RCNT_DELAY_CNT) {
            _delay_ms(4);
            rcnt++;
        }
        if (rcnt >= RCNT_DELAY_CNT)
            state.cur_state = trError();
    }
    
// Handle special states during boot
    if (state.cur_state == ST_NONE) { // Like ST_NONE which are 1st run or reset from error
        v_calibrate(); // so we run calibrate
        SET_LED(GREEN);
        // save_settings(SAVE_FULL);
    } else if (state.cur_state == ST_MAINTENANCE) { // or power failure during maintenance we enter error state for manual inspection
        state.cur_state = trError();
    } else {
        switch (state.cur_state) {
            case ST_NORMAL:
                SET_LED(GREEN);
                break;
            case ST_BYPASS:
                SET_LED(BLUE);
                break;
            case ST_VALVE_ERR:
                SET_LED(RED);
                break;
            default:
                break;
        }
    }

    EINT_ENABLE();

    while(1)
    {
        state.event = fsGetEvent();
        if (state.event != EV_NONE)
            fsTransition();
        if (!state.flags.reed && !GET_REED() && (state.cur_state == ST_NORMAL || state.cur_state == ST_BYPASS)) // Poll reed sensor for short
            state.flags.reed = true;
        else if (state.cur_state == ST_REED_OVERFLOW && GET_REED()) // Poll reed sensor for restoration
            state.flags.restoration = true;

        if (bit_is_set(UCSRA, RXC)) // Poll UART for incoming bytes;
            UART_rx();

        if (state.cur_state == ST_WATER_CLOSED || state.cur_state == ST_REED_OVERFLOW || state.cur_state == ST_RESTORATION) // Work on LED blinking modes
            led_blink();
    }
}
