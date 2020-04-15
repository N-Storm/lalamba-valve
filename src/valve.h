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
typedef enum {VST_CLOSED, VST_MIDDLE, VST_OPEN, VST_ERROR} eValveState;
// Valve move direction
typedef enum {MV_V1_OPEN, MV_V1_CLOSE, MV_V2_OPEN, MV_V2_CLOSE} eValveMove;
// Valve action for vX_setdir() functions
typedef enum {ACT_CLOSE, ACT_OPEN, ACT_BREAK, ACT_STOP} eValveAction;
/* Return/error codes
 * RET_ALREADY_POSITIONED - was already in place where we requested to move (based on SW)
 * RET_MOVED - succesfully moved to a new position
 * RET_OK - generic OK
 */
typedef enum {RET_NONE, RET_ALREADY_POSITIONED, RET_MOVED, RET_TIMEOUT, RET_OK, RET_ERROR} eRetCode;

// Function prototypes
void update_valve_astates();
void v1_setdir(eValveAction dir);
void v2_setdir(eValveAction dir);
eRetCode v_move(eValveMove move);

// Strings for logging
#ifdef LOGS
FLASHSTR STR_ERROR[] = "ERROR!\r\n";
FLASHSTR STR_DONE[] = "done.\r\n";
FLASHSTR STR_TO_OPEN[] =  " to OPEN ";
FLASHSTR STR_TO_CLOSED[] =  " to CLOSED ";
FLASHSTR STR_APOS[] = "already positioned!\r\n";
FLASHSTR STR_FROM_CLOSED[] = "from CLOSED... ";
FLASHSTR STR_FROM_OPEN[] = "from OPEN... ";
FLASHSTR STR_FROM_MIDDLE[] = "from MIDDLE... ";
FLASHSTR STR_TIMEOUT[] = " TIMEOUT hit.\r\n";
#endif

#endif	/* VALVE_H */
