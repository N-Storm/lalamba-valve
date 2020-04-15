/*
 * Project: lalamba-valve
 * File:   saveload.c
 * Author: NStorm
 * Created: 10.04.2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <util/crc16.h>
#include "saveload.h"
#include "main.h"

uint8_t crc8(uint8_t *dataptr, size_t sz) {
    uint8_t crc = 0, i;
    for (i = 0; i < sz; i++)
        crc = _crc8_ccitt_update(crc, dataptr[i]);
    return crc;
}

uint16_t crc16(uint8_t *dataptr, size_t sz) {
    uint16_t crc = 0, i;
    LOG("Settings dump: ");
    for (i = 0; i < sz; i++) {
        crc = _crc16_update(crc, dataptr[i]);
        LOG("0x%X ", dataptr[i]);
    }
    LOG("\r\n");
    return crc;
}

void save_settings() {
    LOG("Saving settings.\r\n");
    settings.state = &state;
    settings.crc8 = 0;
    settings.crc16 = 0;
    settings.crc8 = crc8((void *)&state, sizeof(state));
    settings.crc16 = crc16((void *)&state, sizeof(state));
    LOG("Settings size = %d, CRC16 = %X, CRC8 = %X.\r\n", sizeof(state), settings.crc16, settings.crc8);
}
