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

static settings_t settings = {.crc1 = 0, .crc2 = 0};

// Calculate CRC16 from a buffer dataptr with sz size
uint16_t crc16(uint8_t *dataptr, size_t sz) {
    uint16_t crc = 0;
    uint8_t cnt = sz;

#ifdef VERBOSE_LOGS
    LOG("State dump: ");
#endif
    while(cnt) {
#ifdef VERBOSE_LOGS
        LOG("0x%X ", *dataptr);
#endif
        crc = _crc16_update(crc, *dataptr);
        dataptr++;
        cnt--;
    }
    LOG("\r\n");
    return crc;
}

void load_settings() {
    uint8_t *ptr = 0; // byte pointer in EEPROM
    state_t state_buf;
    EINT_DISABLE();

    LOG("Load settings\r\n");
    settings.seq = eeprom_read_dword((void *)ptr);
    ptr += sizeof(settings.seq);
    eeprom_read_block(&state_buf, (void *)ptr, sizeof(state_buf));
    ptr += sizeof(state_buf);
    settings.crc1 = eeprom_read_word((void *)ptr);
    ptr += sizeof(settings.crc1);
    settings.crc2 = eeprom_read_word((void *)ptr);
    ptr += sizeof(settings.crc2);
    
    if (settings.crc1 == crc16((void *)&state_buf, sizeof(state_buf))) {
        state = state_buf;
        LOG("CRC1 correct\r\n");
    } else
        LOG("CRC1 incorrect\r\n");
}

void save_settings(eSaveMode savemode) {
    LOG("Saving settings, mode = %d\r\n", savemode);
    if (savemode == SAVE_FULL || savemode == SAVE_CRC1)
        settings.crc1 = crc16((void *)&state, sizeof(state)); // Calculate crc16 of state struct
    if (settings.crc1 && (savemode == SAVE_FULL || savemode == SAVE_CRC2)) {
        settings.crc2 = _crc16_update(settings.crc1, (uint8_t)(settings.crc1 >> 8)); // crc2 = crc of state struct + crc1. 1st byte of CRC1
        settings.crc2 = _crc16_update(settings.crc2, (uint8_t)settings.crc1); // and 2nd byte of CRC1
    }
#ifdef VERBOSE_LOGS
    LOG("Settings size = %d, CRC1 = %X, CRC2 = %X.\r\n", sizeof(state) + sizeof(settings), settings.crc1, settings.crc2);
#endif
    // Save data to EEPROM

    // Reset CRCs when save are complete
    if (savemode == SAVE_FULL || savemode == SAVE_CRC2) {
        settings.crc1 = 0;
        settings.crc2 = 0;
    }
}
