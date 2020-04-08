/* 
 * Project: lalamba-valve
 * File:   timers.h
 * Author: NStorm
 * Created on 08.04.2020
 */
#ifndef TIMERS_H
#define	TIMERS_H

// Timeout for full valve rotation in ms
#define V_ROT_TIMEOUT 5000 // 5s
#define V_ROT_TICKS (V_ROT_TIMEOUT/(1/(F_CPU/1024))/1000) // total timer ticks for rotation with 1/1024 prescaler

#endif	/* TIMERS_H */

