/* 
 * Project: lalamba-valve
 * File:   main.h
 * Author: NStorm
 * Created on 04.04.2020
 */
#ifndef MAIN_H
#define	MAIN_H

// Settings
// Delay for valve motor short break in ms
#define V_BREAK_DELAY 200
// Delay for back-and-forth calibration
#define V_BF_DELAY 300
// Delay before rechecking valve switch at the end of forward moving. If it's not hit for some reason after that time (switch bounce), we restart movement from back-and-forth action until timout
#define V_RESTART_DELAY 10
// Reset on boot up with button held delay counter (x 4ms), see main())
#define RCNT_DELAY_CNT 250

// Debugging options
#ifdef DEBUG
// Set to enable printf() debug messages on UART
#define LOGS
// Set to enable even more log messages.
#define VERBOSE_LOGS
#endif

#define FLASHSTR static PROGMEM const char

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
#define MDRV_SLP_PORT PORTC
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
#define RED {0, 128, 0}
#define RED_HALF {0, 64, 0}
#define GREEN {64, 0, 0}
#define GREEN_HALF {16, 0, 0}
#define BLUE {0, 0, 64}
#define BLUE_HALF {0, 0, 16}
#define YELLOW {128, 128, 0}
#define YELLOW_HALF {32, 32, 0}
#define VIOLET {0, 32, 64}
#define VIOLET_HALF {0, 8, 16}

#include "valve.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Enums
typedef enum {ST_NONE, ST_NORMAL, ST_BYPASS, ST_REED_OVERFLOW, ST_RESTORATION, ST_MAINTENANCE, ST_WATER_CLOSED, ST_VALVE_ERR, ST_ANY, ST_LAST} eState;
typedef enum {EV_NONE, EV_BTN_SHORT, EV_BTN_LONG, EV_BTN_EXTRA_LONG, EV_REED, EV_REED_RESTORATION, EV_AC_RESTORATION, EV_VALVE_TIMEOUT, EV_ANY, EV_LAST} eEvent;
typedef enum {BTN_NONE, BTN_PRESSED, BTN_SHORT, BTN_LONG, BTN_EXTRA_LONG} eBtnState;

// Struct types
typedef struct {
    bool error : 1;
    bool reed : 1;
    bool restoration : 1;
    bool ac_shortage : 1;
    bool ac_restored : 1;
} flags_t;

/*  valveX_astate - actual state based on switches
 *  valveX_sstate - software defined state
 */
typedef struct {
    eState cur_state;
    eState prev_state;
    eEvent event;
    eBtnState btn_state;
    flags_t flags;
    eValveState v1_state;
    eValveState v2_state;
} state_t;

// Globals
extern volatile state_t state;
// extern settings_t settings;
extern FILE mystdout;

// Function prototypes
void v_calibrate();

// Macro "functions"
// Enable INT0, INT1
#define EINT_ENABLE() do { GICR = (1 << INT1) | (1 << INT0); } while (0)
// Disable INT0, INT1 
#define EINT_DISABLE() do { GICR = 0; } while (0);
// Check reed sensor reading. Return true if reed is HIGH (normal).
#define GET_REED() bit_is_set(REED_PIN, REED)
#define SET_LED(COLOR) do { struct cRGB leda = COLOR; ws2812_setleds(&leda, 1); } while (0)

// Logging via UART
#ifdef LOGS
#define LOG(MSG, ...) do { printf_P(PSTR(MSG), ##__VA_ARGS__); } while(0)
#define LOGP(MSG) do { printf_P(MSG); } while(0)
#else
#define LOG(...) do { } while(0)
#define LOGP(...) do { } while(0)
#endif

// Pseudo-function to jump to a bootloader
typedef void (*ptr_t)(void);
extern const ptr_t bootloader_start;

#endif	/* MAIN_H */
