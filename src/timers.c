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

volatile bool timeout_flag = false;

ISR(TIMER0_OVF_vect) {
    static uint16_t ovf;
    ovf++;
    if (ovf < V_ROT_OVF)
        return;
    else if (ovf == V_ROT_OVF) {
        TCCR0 = 0;
        TCNT0 = 255-V_ROT_REM;
        SFIOR |= _BV(PSR10);
        TCCR0 = _BV(CS02) | _BV(CS00);
    }
    else {
        TCCR0 = 0;
        TCNT0 = 0;
        ovf = 0;
        timeout_flag = true;
    }
}
