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

// Struct for v_move() which stores parsed movements parameters, like directions ACT, etc
typedef struct {
    eValveAction fact;
    eValveAction ract;
    uint8_t pinbit;
    uint8_t slppin;
    void (*setdir)(eValveAction);
    volatile eValveState *vstate;
    eValveState endstate;
} moves_t;

// Function prototypes
eRetCode v_move(eValveMove move);
void v_update_states();
void v_calibrate();

// Strings for logging
#ifdef LOGS
FLASHSTR STR_ERROR[] = "ERROR!\r\n";
FLASHSTR STR_DONE[] = "done.\r\n";
FLASHSTR STR_CLOSED[] = "CLOSED";
FLASHSTR STR_OPEN[] = "OPEN";
FLASHSTR STR_MIDDLE[] = "MIDDLE";
FLASHSTR STR_APOS[] = "Already positioned!\r\n";
FLASHSTR STR_TIMEOUT[] = "TIMEOUT hit.\r\n";
#endif

#endif	/* VALVE_H */
