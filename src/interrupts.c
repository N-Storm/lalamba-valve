/*
 * Project: lalamba-valve
 * File:   interrupts.c
 * Author: NStorm
 * Created: 04.04.2020
 */
#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "main.h"
#include "timers.h"

// Button handling ISR. Changes btn_state based on timer0 & button release
ISR(INT0_vect) {
    if (bit_is_clear(BTN_PIN, BTN) && state.btn_state == BTN_NONE) { // Set button state & run timer on button press
        STOP_TIMEOUT(); // To prevent bounce
        state.btn_state = BTN_PRESSED;
        RUN_TIMEOUT(BTN_LONG_OVF); // Run timeout for LONG overflow period (more than LONG press timing)
#ifdef VERBOSE_LOGS
        LOG("BTN press\r\n");
#endif
    } else if (bit_is_set(BTN_PIN, BTN) && state.btn_state == BTN_PRESSED) { // Things to check on button release
#ifdef VERBOSE_LOGS
        LOG("BTN release!\r\n");
#endif
        if ((t0_ovf_cnt < BTN_LONG_OVF) && (t0_ovf_cnt > (BTN_LONG_OVF - BTN_SHORT_OVF)) && !t0_timeout_flag) // if at least 1 overflow period has passed & no timeout yet, but less than short period has passed
            state.btn_state = BTN_SHORT;
        else if ((t0_ovf_cnt <= (BTN_LONG_OVF - BTN_SHORT_OVF)) && !t0_timeout_flag) // if overflow counter has already counted more than BTN_SHORT_OVF
            state.btn_state = BTN_LONG;
        else if (t0_timeout_flag)
            state.btn_state = BTN_EXTRA_LONG;
        else
            state.btn_state = BTN_NONE;
        STOP_TIMEOUT();
        t0_timeout_flag = false;
    } 
#ifdef VERBOSE_LOGS
    else {
        LOG("BTN bounce, btn_state = %d, btn status = ");
        if (BTN_PIN & (1 << BTN))
            LOG("UNPRESSED.\r\n");
        else
            LOG("PRESSED.\r\n");
    }
#endif
}

// AC detector handlong. Sets ac_shortage flag on AC shortage & later sets ac_restored flag on restoration
ISR(INT1_vect) {
    if (bit_is_set(ACDET_PIN, ACDET)) {
        state.flags.ac_shortage = true;
        LOG("PWR outage\r\n");
    } else if (bit_is_clear(ACDET_PIN, ACDET) && state.flags.ac_shortage) {
        state.flags.ac_restored = true;
        state.flags.ac_shortage = false;
        LOG("PWR restored\r\n");
    }
}
