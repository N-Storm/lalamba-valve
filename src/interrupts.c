/* 
 * Project: lalamba-valve
 * File:   interrupts.c
 * Author: NStorm
 * Created: 04.04.2020 21:06
 */
#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "main.h"
#include "timers.h"

ISR(INT0_vect) {
    if (bit_is_clear(BTN_PIN, BTN)) {
        state.btn_state = BTN_PRESSED;
        RUN_TIMEOUT(BTN_SHORT_OVF);
        LOG("BTN press\r\n");
    } else if (bit_is_set(BTN_PIN, BTN) && state.btn_state == BTN_PRESSED) {
        if (t0_ovf_cnt < BTN_SHORT_OVF - 1 && !t0_timeout_flag)
            state.btn_state = BTN_SHORT;
        else if (t0_timeout_flag)
            state.btn_state = BTN_LONG;
        else
            state.btn_state = BTN_NONE;
        STOP_TIMEOUT();
        t0_timeout_flag = false;
    } else
        state.btn_state = BTN_NONE;
}

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
