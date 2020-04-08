/* 
 * Project: lalamba-valve
 * File:   timers.h
 * Author: NStorm
 * Created on 08.04.2020
 */
#include <avr/io.h>

#ifndef TIMERS_H
#define	TIMERS_H

// Timeout for full valve rotation in ms
#define V_ROT_TIMEOUT 5000 // 5s
#define V_ROT_TICKS V_ROT_TIMEOUT/(1.0/(F_CPU/1024.0))/1000.0 // Total timer ticks for rotation with 1/1024 prescaler
#define V_ROT_OVF (uint16_t)(V_ROT_TICKS/256) // Total timer overflow count
#define V_ROT_REM (uint8_t)(V_ROT_TICKS-((uint32_t)V_ROT_OVF*256)) // Timer ticks remaining

#endif	/* TIMERS_H */