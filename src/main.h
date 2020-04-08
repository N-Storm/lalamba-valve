/* 
 * Project: lalamba-valve
 * File:   main.h
 * Author: NStorm
 * Created on 04.04.2020, 21:22
 */

#ifndef MAIN_H
#define	MAIN_H

// IO Mappings (PIN defines)
// PORT B
#define WS2812_PORT PORTB
#define WS2812_PIN PB0
#define AIN_2_PORT PORTB
#define AIN1_2_PORT PORTB
#define AIN1_2_PIN PB1
#define AIN2_2_PORT PORTB
#define AIN2_2_PIN PB2
#define PWMA_PORT PORTB
#define PWMA_PIN PB3

// PORT C
#define AIN_PORT PORTC
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
#define MSW_PORT PORTD
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

// Enums
// Current status of the valve
typedef enum {VALVE_CLOSED, VALVE_MIDDLE, VALVE_OPEN, VALVE_ERROR} eValveState;
// Valve move direction
typedef enum {V1_OPEN, V1_CLOSE, V2_OPEN, V2_CLOSE} eValveMove;
/* Return/error codes
 * ALREADY_POSITIONED - was already in place where we requested to move (based on SW)
 * MOVED - succesfully moved to a new position
 */
typedef enum {NONE, ALREADY_POSITIONED, MOVED} eRetCode;
    
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
extern struct cRGB leda;
extern volatile state_t state;
extern settings_t settings;

// Function prototypes
eRetCode v_move(eValveMove move);

// Macro "functions"
// Enable INT0, INT1
#define EINT_ENABLE() do { GICR = (1 << INT1) | (1 << INT0); } while (0);
// Disable INT0, INT1 
#define EINT_DISABLE() do { GICR = 0; } while (0);
// Check reed sensor reading. Return true if reed is HIGH (normal).
#define GET_REED() bit_is_set(REED_PORT, REED_PIN);

#endif	/* MAIN_H */
