/*
 * Project: lalamba-valve
 * File:   valve.c
 * Author: NStorm
 * Created: 10.04.2020
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
#include "valve.h"
#include "timers.h"
#include "saveload.h"

// Updates actual (based on switches) valve states
void v_update_states() {
    LOG("Checking valve states.\r\n");
    if (bit_is_clear(MSW_PIN, M1SW1) && bit_is_set(MSW_PIN, M1SW2))
        state.v1_state = VST_CLOSED;
    else if (bit_is_set(MSW_PIN, M1SW1) && bit_is_clear(MSW_PIN, M1SW2))
        state.v1_state = VST_OPEN;
    else
        state.v1_state = VST_MIDDLE;

    if (bit_is_clear(MSW_PIN, M2SW1) && bit_is_set(MSW_PIN, M2SW2))
        state.v2_state = VST_CLOSED;
    else if (bit_is_set(MSW_PIN, M2SW1) && bit_is_clear(MSW_PIN, M2SW2))
        state.v2_state = VST_OPEN;
    else
        state.v2_state = VST_MIDDLE;
}

// Helper function for v_move() to prepare ports before actually running the driver for valve1
void static inline v1_setdir(eValveAction dir) {
    switch (dir) {
        case ACT_CLOSE:
            AIN1_PORT |= _BV(AIN1);
            AIN2_PORT &= ~_BV(AIN2);
            PWMA_PORT |= _BV(PWMA);
            break;
        case ACT_OPEN:
            AIN1_PORT &= ~_BV(AIN1);
            AIN2_PORT |= _BV(AIN2);
            PWMA_PORT |= _BV(PWMA);
            break;
        case ACT_BREAK:
            PWMA_PORT &= ~_BV(PWMA);
            AIN1_PORT |= _BV(AIN1);
            AIN2_PORT |= _BV(AIN2);
            _delay_ms(V_BREAK_DELAY);
            break;
        case ACT_STOP:
            PWMA_PORT &= ~_BV(PWMA);
            AIN1_PORT &= ~_BV(AIN1);
            AIN2_PORT &= ~_BV(AIN2);
            break;
    }
}

// Helper function for v_move() to prepare ports before actually running the driver for valve2
void static inline v2_setdir(eValveAction dir) {
    switch (dir) {
        case ACT_CLOSE:
            AIN1_2_PORT |= _BV(AIN1_2);
            AIN2_2_PORT &= ~_BV(AIN2_2);
            break;
        case ACT_OPEN:
            AIN1_2_PORT &= ~_BV(AIN1_2);
            AIN2_2_PORT |= _BV(AIN2_2);
            break;
        case ACT_BREAK:
            AIN1_2_PORT |= _BV(AIN1_2);
            AIN2_2_PORT |= _BV(AIN2_2);
            _delay_ms(V_BREAK_DELAY);
            break;
        case ACT_STOP:
            AIN1_2_PORT &= ~_BV(AIN1_2);
            AIN2_2_PORT &= ~_BV(AIN2_2);
            break;
    }
}

/* Check current valve pos in accordance with move direction to detect ALREADY_POSITIONED error
 * Call to v_update_states() at the beggning to read switch actual positions
*/ 
 eRetCode static inline v_check_pos(eValveMove move) {
    v_update_states();
    switch (move) {
        case MV_V1_OPEN:
            if (state.v1_state == VST_OPEN)
                return RET_ALREADY_POSITIONED;
            break;
        case MV_V1_CLOSE:
            if (state.v1_state == VST_CLOSED)
                return RET_ALREADY_POSITIONED;
            break;
        case MV_V2_OPEN:
            if (state.v2_state == VST_OPEN)
                return RET_ALREADY_POSITIONED;
            break;
        case MV_V2_CLOSE:
            if (state.v2_state == VST_CLOSED)
                return RET_ALREADY_POSITIONED;
            break;
    }
    return RET_OK;
}

void static inline v_log_direction(eValveMove move) {
        LOG("Moving valve ");
        if (move == MV_V1_CLOSE || move == MV_V1_OPEN)
            LOG("1 ");
        else
            LOG("2 ");
        if ((state.v1_state == VST_CLOSED && move == MV_V1_OPEN) || (state.v2_state == VST_CLOSED && move == MV_V2_OPEN))
            LOGP(STR_CLOSED);
        else if ((state.v1_state == VST_OPEN && move == MV_V1_CLOSE) || (state.v2_state == VST_OPEN && move == MV_V2_CLOSE))
            LOGP(STR_OPEN);
        else
            LOGP(STR_MIDDLE);
        LOG("->");
        if (move == MV_V1_OPEN || move == MV_V2_OPEN)
            LOGP(STR_OPEN);
        else
            LOGP(STR_CLOSED);
        LOG(" ");
}

// Populate moves_t struct values based on eValveMove
void static inline v_parse_move(eValveMove move, moves_t *moves) {
    if (move == MV_V1_OPEN || move == MV_V2_OPEN) {
        moves->fact = ACT_OPEN;
        moves->ract = ACT_CLOSE;
        moves->endstate = VST_OPEN;
    } else {
        moves->fact = ACT_CLOSE;
        moves->ract = ACT_OPEN;
        moves->endstate = VST_CLOSED;
    }
    if (move == MV_V1_OPEN || move == MV_V1_CLOSE) {
        moves->setdir = &v1_setdir;
        moves->vstate = &state.v1_state;
        moves->slppin = STBY;
    } else {
        moves->setdir = &v2_setdir;
        moves->vstate = &state.v2_state;
        moves->slppin = NSLEEP;
    }
    
    switch (move) {
        case MV_V1_OPEN:
            moves->pinbit = M1SW2;
            break;
        case MV_V1_CLOSE:
            moves->pinbit = M1SW1;
            break;
        case MV_V2_OPEN:
            moves->pinbit = M2SW2;
            break;
        case MV_V2_CLOSE:
            moves->pinbit = M2SW1;
            break;
    }
}

// Function which actually moves valve.
eRetCode v_move(eValveMove move) {
    eRetCode ret = RET_NONE;
    moves_t moves;

    // Check valve position before moving.
    if (v_check_pos(move) == RET_ALREADY_POSITIONED) {
        state.flags.error = true;
        LOGP(STR_APOS);
        return RET_ALREADY_POSITIONED;
    }

#ifdef LOGS
    v_log_direction(move);
#endif

    v_parse_move(move, &moves);

    EINT_DISABLE();

// If valve state saved as MIDDLE, when we move it a little backwards before going forward
    if (*moves.vstate == VST_MIDDLE) {
        moves.setdir(moves.ract);
        MDRV_SLP_PORT |= _BV(moves.slppin); // Run motor  
        _delay_ms(V_BF_DELAY);
        moves.setdir(ACT_BREAK);
    }

// Main direction movement block
    *moves.vstate = VST_MIDDLE;
    moves.setdir(moves.fact);
    MDRV_SLP_PORT |= _BV(moves.slppin); // Run motor    
    RUN_TIMEOUT(V_ROT_OVF_SIMPLE);
    while (bit_is_set(MSW_PIN, moves.pinbit) && !t0_timeout_flag); // Wait until SW are hit by motor
    STOP_TIMEOUT();
    moves.setdir(ACT_BREAK);
    moves.setdir(ACT_STOP);
    MDRV_SLP_PORT &= ~_BV(moves.slppin); // Go back to STBY
    if (t0_timeout_flag) { // Valve movement timeout hit
        LOGP(STR_TIMEOUT);
        t0_timeout_flag = false;
        state.flags.error = true;
        *moves.vstate = VST_MIDDLE;
        ret = RET_TIMEOUT;
    } else { // Move successful
        *moves.vstate = moves.endstate;
        LOGP(STR_DONE);
        ret = RET_MOVED;
    }

    EINT_ENABLE();
    return ret;
}

void v_calibrate() {
    LOG("Calibration begin.\r\n");
    v_update_states();

    if (state.v1_state != VST_CLOSED)
        v_move(MV_V1_CLOSE);

    if (state.v2_state != VST_OPEN)
        v_move(MV_V2_OPEN);

    state.cur_state = ST_NORMAL;
    LOG("Calibration done.\r\n");
}
