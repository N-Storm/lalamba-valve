/*
 * Project: lalamba-valve
 * File:   timers.h
 * Author: NStorm
 * Created on 08.04.2020
 */
#include <avr/io.h>

#ifndef TIMERS_H
#define	TIMERS_H

// Timeout for full valve rotation in ms. Only simple MAX 8 bit overflow are used now.
#define V_ROT_TIMEOUT 6000 // 6s
#define V_ROT_TICKS V_ROT_TIMEOUT/(1.0/(F_CPU/1024.0))/1000.0 // Total timer ticks for rotation with /1024 prescaler
#define V_ROT_OVF (uint16_t)(V_ROT_TICKS/256) // Total timer overflow count
#define V_ROT_REM (uint8_t)(V_ROT_TICKS-((uint32_t)V_ROT_OVF*256)) // Timer ticks remaining
#define V_ROT_OVF_SIMPLE V_ROT_OVF

// #define V_ROT_OVF_SIMPLE 255 // ~4.1s to fit 8 bit var

// MAX time for short button press
#define BNT_SHORT_MAX 1000 // 1s
#define BTN_SHORT_TICKS BNT_SHORT_MAX/(1.0/(F_CPU/1024.0))/1000.0 // Total timer ticks for rotation with /1024 prescaler
#define BTN_SHORT_OVF (uint8_t)(BTN_SHORT_TICKS/256) // Total timer overflow count
// #define BTN_SHORT_REM (uint8_t)(BTN_SHORT_TICKS-((uint32_t)BTN_SHORT_OVF*256)) // Timer ticks remaining

// MAX time for long button press
#define BTN_LONG_MAX 4000 // 4s (total)
#define BTN_LONG_TICKS BTN_LONG_MAX/(1.0/(F_CPU/1024.0))/1000.0 // Total timer ticks for rotation with /1024 prescaler
#define BTN_LONG_OVF (uint8_t)(BTN_LONG_TICKS/256) // Total timer overflow count
// #define BTN_LONG_REM (uint8_t)(BTN_LONG_TICKS-((uint32_t)BTN_LONG_OVF*256)) // Timer ticks remaining

// Globals
extern volatile bool t0_timeout_flag;
extern volatile uint16_t t0_ovf_cnt;

// Run timer with /1024 PS
#define RUN_TIMEOUT(OVF) do { TCNT0 = 0; t0_ovf_cnt = OVF; SFIOR |= _BV(PSR10); TCCR0 = _BV(CS02) | _BV(CS00); } while (0)
#define STOP_TIMEOUT() do { TCCR0 = 0; TCNT0 = 0; t0_ovf_cnt = 0; } while (0)

#endif	/* TIMERS_H */
