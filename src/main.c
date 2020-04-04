/* 
 * File:   main.c
 * Author: NStorm
 * Created: 04.04.2020 21:06
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "main.h"
#include "ws2812_config.h"
#include "light_ws2812.h"

// Array for WS2818 LED colors
struct cRGB leda = WHITE;

void inline init() {
    // IO settings
    DDRB = _BV(WS2812_PIN) | _BV(AIN1_2_PIN) | _BV(AIN2_2_PIN) | _BV(PWMA_PIN);
    DDRC = _BV(AIN1_PIN) | _BV(AIN2_PIN) | _BV(STBY_PIN) | _BV(NSLEEP_PIN);
    DDRD = _BV(TX_PIN) | _BV(INT1_PIN) | _BV(M1SW1_PIN) | _BV(M1SW2_PIN) | _BV(M2SW1_PIN) | _BV(M2SW2_PIN);
    
    ws2812_setleds(&leda, 1);
}
    
int main(void)
{
    init();
    
    while (1) 
    {
    }
}
