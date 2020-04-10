/* 
 * Project: lalamba-valve
 * File:   valve.h
 * Author: NStorm
 * Created: 10.04.2020
 */
#ifndef VALVE_H
#define	VALVE_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/cpufunc.h>
#include <util/delay.h>
#include "main.h"

// Enums
// Current status of the valve
typedef enum {VALVE_CLOSED, VALVE_MIDDLE, VALVE_OPEN, VALVE_ERROR} eValveState;
// Valve move direction
typedef enum {V1_OPEN, V1_CLOSE, V2_OPEN, V2_CLOSE} eValveMove;
/* Return/error codes
 * ALREADY_POSITIONED - was already in place where we requested to move (based on SW)
 * MOVED - succesfully moved to a new position
 */
typedef enum {NONE, ALREADY_POSITIONED, MOVED, ERROR} eRetCode;
// Valve action for vX_setdir() functions
typedef enum {CLOSE, OPEN, BREAK, STOP} eValveAction;

// Function prototypes
void update_valve_astates();
void v1_setdir(eValveAction dir);
void v2_setdir(eValveAction dir);
eRetCode v_move(eValveMove move);


#endif	/* VALVE_H */

