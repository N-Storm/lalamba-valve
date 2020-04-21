/*
 * Project: lalamba-valve
 * File:   saveload.h
 * Author: NStorm
 * Created: 10.04.2020
 */
#ifndef SAVELOAD_H
#define	SAVELOAD_H

#include "main.h"

// Defines
#define EEPROM_SIZE 512
#define EEPROM_ENTRY_SIZE sizeof(settings_t) + sizeof(state_t)
#define EEPROM_ENTRIES (EEPROM_SIZE / EEPROM_ENTRY_SIZE)

// Enums
typedef enum {SAVE_FULL, SAVE_CRC1, SAVE_CRC2} eSaveMode;

// Function prototypes
void save_settings(eSaveMode savemode);
void load_settings();

// Structs
typedef struct {
    uint32_t seq;
    // state_t goes here in actual EEPROM
    uint16_t crc1;
    uint16_t crc2;
} settings_t;

#endif	/* SAVELOAD_H */
