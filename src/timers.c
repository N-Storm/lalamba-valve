/*
 * Project: lalamba-valve
 * File:   timers.c
 * Author: NStorm
 * Created on 08.04.2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/cpufunc.h>
#include <util/delay.h>

#include "main.h"
#include "timers.h"
#include "valve.h"

volatile bool t0_timeout_flag = false;
volatile uint8_t t0_ovf_cnt;

// Simple counter which runs from RUN_TIMEOUT(OVF) macro which sets t0_ovf_cnt.
ISR(TIMER0_OVF_vect) {
    t0_ovf_cnt--;
    if (t0_ovf_cnt == 0) {
        TCCR0 = 0;
        TCNT0 = 0;
        t0_timeout_flag = true;
    }
}
