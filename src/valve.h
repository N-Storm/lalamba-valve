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
#include <avr/pgmspace.h>
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

// Strings for logging
#define FLASHSTR static PROGMEM const char
FLASHSTR STR_ERROR[] = "ERROR!\r\n";
FLASHSTR STR_DONE[] = "done.\r\n";
FLASHSTR STR_TO_OPEN[] =  " to OPEN ";
FLASHSTR STR_TO_CLOSED[] =  " to CLOSED ";
FLASHSTR STR_APOS[] = "already positioned!\r\n";
FLASHSTR STR_FROM_CLOSED[] = "from CLOSED... ";
FLASHSTR STR_FROM_OPEN[] = "from OPEN... ";
FLASHSTR STR_FROM_MIDDLE[] = "from MIDDLE... ";
FLASHSTR STR_TIMEOUT[] = " TIMEOUT hit ";

#endif	/* VALVE_H */
