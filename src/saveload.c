/*
 * Project: lalamba-valve
 * File:   saveload.c
 * Author: NStorm
 * Created: 10.04.2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <util/crc16.h>
#include <avr/eeprom.h>
#include "saveload.h"
#include "main.h"

// Calculate CRC16 from a buffer dataptr with sz size
uint16_t crc16(uint8_t *dataptr, size_t sz) {
    uint16_t crc = 0;
    uint8_t cnt = sz;

#ifdef VERBOSE_LOGS
    LOG("Settings dump: ");
#endif
    while(cnt) {
        crc = _crc16_update(crc, *dataptr);
        dataptr++;
        cnt--;
#ifdef VERBOSE_LOGS
        LOG("0x%X ", *dataptr);
#endif
    }
    LOG("\r\n");
    return crc;
}

void save_settings() {
    settings_t settings = {.crc1 = 0, .crc2 = 0};
    LOG("Saving settings.\r\n");
    settings.crc1 = crc16((void *)&state, sizeof(state)); // Calculate crc16 of state struct
    settings.crc2 = _crc16_update(settings.crc1, (uint8_t)(settings.crc1 >> 8)); // crc2 = crc of state struct + crc1. 1st byte of CRC1
    settings.crc2 = _crc16_update(settings.crc2, (uint8_t)settings.crc1); // and 2nd byte of CRC1
#ifdef VERBOSE_LOGS
    LOG("Settings size = %d, CRC1 = %X, CRC2 = %X.\r\n", sizeof(state) + sizeof(settings), settings.crc1, settings.crc2);
#endif
}
