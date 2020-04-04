/* 
 * File:   main.h
 * Author: NStorm
 * Created on 04.04.2020, 21:22
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

// IO Mappings (PIN defines)
// PORT B
#define WS2812_PORT PORTB
#define WS2812_PIN PB0
#define AIN1_2_PORT PORTB
#define AIN1_2_PIN PB1
#define AIN2_2_PORT PORTB
#define AIN2_2_PIN PB2
#define PWMA_PORT PORTB
#define PWMA_PIN PB3

// PORT C
#define AIN1_PORT PORTC
#define AIN1_PIN PC0
#define AIN2_PORT PORTC
#define AIN2_PIN PC2
#define STBY_PORT PORTC
#define STBY_PIN PC3
#define NSLEEP_PORT PORTC
#define NSLEEP_PIN PC4
#define REED_PORT PORTC
#define REED_PIN PC5

// PORT D
#define RX_PORT PORTD
#define RX_PIN PD0
#define TX_PORT PORTD
#define TX_PIN PD1
#define BTN_PORT PORTD
#define BTN_PIN PD2
#define INT1_PORT PORTD
#define INT1_PIN PD3
#define M1SW1_PORT PORTD
#define M1SW1_PIN PD4
#define M1SW2_PORT PORTD
#define M1SW2_PIN PD5
#define M2SW1_PORT PORTD
#define M2SW1_PIN PD6
#define M2SW2_PORT PORTD
#define M2SW2_PIN PD7

// LED colors
#define WHITE {255, 255, 255}
#define BLACK {0, 0, 0}
#define RED {0, 255, 0}
#define GREEN {255, 0, 0}
#define BLUE {0, 0, 255}
#define YELLOW {255, 255, 0}
#define VIOLET {0, 165, 255}

// Struct types
typedef struct {
    uint8_t current_state;
    uint8_t prev_state;
} state_t;

typedef struct {
    volatile state_t *state;
    uint16_t crc16;
    uint8_t crc8;
} settings_t;

// Globals
struct cRGB leda;
volatile state_t state;
settings_t settings;

#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

