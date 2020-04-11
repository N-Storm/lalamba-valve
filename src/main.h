/* 
 * Project: lalamba-valve
 * File:   main.h
 * Author: NStorm
 * Created on 04.04.2020, 21:22
 */
#ifndef MAIN_H
#define	MAIN_H

// Settings
// Delay for valve motor short break in ms
#define V_SHORT_DELAY 100
// Delay for back-and-forth calibration
#define V_BF_DELAY 100
// Set to enable printf() debug messages on UART
#define LOGS

// IO Mappings (PIN defines)
// PORT B
#define WS2812_PORT PORTB
#define WS2812 PB0
#define AIN_2_PORT PORTB
#define AIN1_2_PORT PORTB
#define AIN1_2 PB1
#define AIN2_2_PORT PORTB
#define AIN2_2 PB2
#define PWMA_PORT PORTB
#define PWMA PB3

// PORT C
#define AIN_PORT PORTC
#define AIN1_PORT PORTC
#define AIN1 PC0
#define AIN2_PORT PORTC
#define AIN2 PC1
#define STBY_PORT PORTC
#define STBY PC2
#define NSLEEP_PORT PORTC
#define NSLEEP PC3
#define REED_PORT PORTC
#define REED_PIN PINC
#define REED PC4

// PORT D
#define RX_PORT PORTD
#define RX_PIN PIND
#define RX PD0
#define TX_PORT PORTD
#define TX PD1
#define BTN_PORT PORTD
#define BTN_PIN PIND
#define BTN PD2
#define ACDET_PORT PORTD
#define ACDET_PIN PIND
#define ACDET PD3
#define MSW_PORT PORTD
#define MSW_PIN PIND
#define M1SW1 PD4
#define M1SW2 PD5
#define M2SW1 PD6
#define M2SW2 PD7

// LED colors GBR
#define WHITE {255, 255, 255}
#define BLACK {0, 0, 0}
#define RED {0, 255, 0}
#define GREEN {255, 0, 0}
#define BLUE {0, 0, 255}
#define YELLOW {255, 255, 0}
#define VIOLET {0, 165, 255}

#include "valve.h"

/* Struct types
 * valveX_astate - actual state based on switches
 * valveX_sstate - software defined state
 */
typedef struct {
    uint8_t prev_state;
    uint8_t current_state;
    uint8_t next_state;
    eValveState v1_astate, v1_sstate;
    eValveState v2_astate, v2_sstate;
} state_t;

typedef struct {
    volatile state_t *state;
    uint16_t crc16;
    uint8_t crc8;
} settings_t;

// Globals
extern volatile state_t state;
extern settings_t settings;
extern FILE mystdout;

// Function prototypes
eRetCode v_move(eValveMove move);

// Macro "functions"
// Enable INT0, INT1
#define EINT_ENABLE() do { GICR = (1 << INT1) | (1 << INT0); } while (0);
// Disable INT0, INT1 
#define EINT_DISABLE() do { GICR = 0; } while (0);
// Check reed sensor reading. Return true if reed is HIGH (normal).
#define GET_REED() bit_is_set(REED_PIN, REED);
#define SET_LED(COLOR) do { struct cRGB leda = COLOR; ws2812_setleds(&leda, 1); } while (0);

// Logging via UART
#ifdef LOGS
#define LOG(MSG, ...) do { printf_P(PSTR(MSG), ##__VA_ARGS__); } while(0)
#define LOGP(MSG) do { printf_P(MSG); } while(0)
#else
#define LOG(MSG) do { } while(0)
#define LOGP(MSG) do { } while(0)
#endif

#endif	/* MAIN_H */
