/*
 * Project: lalamba-valve
 * File:   saveload.h
 * Author: NStorm
 * Created: 10.04.2020
 */
#ifndef SAVELOAD_H
#define	SAVELOAD_H

// Function prototypes
void save_settings();

// Structs
typedef struct {
    uint16_t crc1;
    uint16_t crc2;
} crc_t;

#endif	/* SAVELOAD_H */
