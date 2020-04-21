/*
 * Project: lalamba-valve
 * File:   saveload.h
 * Author: NStorm
 * Created: 10.04.2020
 */
#ifndef SAVELOAD_H
#define	SAVELOAD_H

#include "main.h"


// Function prototypes
void save_settings();

// Structs
typedef struct {
    uint32_t id;
    state_t state;
    uint16_t crc1;
    uint16_t crc2;
} settings_t;

#endif	/* SAVELOAD_H */
