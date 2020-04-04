/* 
 * File:   main.c
 * Author: NStorm
 * Created: 04.04.2020 21:06:00
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

struct cRGB leda = {255, 255, 255};

void init() {
    ws2812_setleds(&leda, 1);
}
    
int main(void)
{
    init();
    
    while (1) 
    {
    }
}
