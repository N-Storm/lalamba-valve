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

ISR(INT0_vect) {
    state.btn_state = BTN_SHORT;
    LOG("BTN press\r\n");    
}

ISR(INT1_vect) {
    state.flags.ac_shortage = true;
    LOG("PWR outage\r\n");
}
