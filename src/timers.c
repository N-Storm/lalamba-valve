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

volatile uint32_t test;

int testf() {
    test = V_ROT_TICKS;
    test = V_ROT_OVF;
    test = V_ROT_REM;
    if (test > 0xAA) return 1;
    return 0;
}
