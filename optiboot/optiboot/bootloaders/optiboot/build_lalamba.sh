#!/bin/bash
make clean; make atmega8 AVR_FREQ=8000000L BAUD_RATE=76800 LED_START_FLASHES=3 BIGBOOT=0 UART=0 LED=B5 SINGLESPEED=0 TIMEOUT_MS=500
