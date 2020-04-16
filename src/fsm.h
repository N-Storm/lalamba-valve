/*
 * Project: lalamba-valve
 * File:   fsm.h
 * Author: NStorm
 * Created: 12.04.2020
 */

#ifndef FSM_H
#define	FSM_H

// Transition table type
typedef eState (*trans_t[ST_LAST][EV_LAST])(void);

// Globals
extern trans_t trans;

// Function prototypes
eEvent fsGetEvent();
eRetCode fsTransition();

#ifdef LOGS
FLASHSTR STR_VALVE_POS_ERROR[] = "Valve position error!\r\n";
#endif

#endif	/* FSM_H */
