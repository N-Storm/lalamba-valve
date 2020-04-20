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

/*
uint8_t crc8(uint8_t *dataptr, size_t sz) {
    uint8_t crc = 0, i;
    for (i = 0; i < sz; i++)
        crc = _crc8_ccitt_update(crc, dataptr[i]);
    return crc;
}

uint16_t crc16(uint8_t *dataptr, size_t sz) {
    uint16_t crc = 0;
    uint8_t i;
    
    LOG("Settings dump: ");
    for (i = 0; i < sz; i++) {
        crc = _crc16_update(crc, dataptr[i]);
        LOG("0x%X ", dataptr[i]);
    }
    LOG("\r\n");
    return crc;
}
*/

uint16_t crc16(uint8_t *dataptr, size_t sz) {
    uint16_t crc = 0;
    uint8_t cnt = sz;
    
    LOG("Settings dump: ");
    while(cnt) {
        crc = _crc16_update(crc, *dataptr);
        dataptr++;
        cnt--;
        LOG("0x%X ", *dataptr);
    }
    LOG("\r\n");
    return crc;
}

void save_settings() {
    crc_t crc = {0, 0};
    LOG("Saving settings.\r\n");
    // settings.crc8 = crc8((void *)&state, sizeof(state));
    crc.crc1 = crc16((void *)&state, sizeof(state)); // Calculate crc16 of state struct
    crc.crc2 = _crc16_update(crc.crc1, crc.crc1 >> 8); // crc2 = crc of state struct + crc1
    crc.crc2 = _crc16_update(crc.crc2, (uint8_t)crc.crc1);
    LOG("Settings size = %d, CRC1 = %X, CRC2 = %X.\r\n", sizeof(state) + sizeof(crc), crc.crc1, crc.crc2);
}
