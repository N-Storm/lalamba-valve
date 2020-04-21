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

static settings_t settings = {.seq = 0, .crc1 = 0, .crc2 = 0};
static uint8_t queue_num = 0;

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
    uint32_t cur_seq = 0;
    state_t state_buf;
    
    cli();
    LOG("Load settings\r\n");

    while(queue_num <= EEPROM_ENTRIES) {
#ifdef VERBOSE_LOGS
        LOG("Reading entry %d\r\n", queue_num);
#endif
        settings.seq = eeprom_read_dword((void *) ptr);
        if (settings.seq <= cur_seq || settings.seq == 0xFFFFFFFF) {
            settings.seq = cur_seq;
            break;
        }
        cur_seq = settings.seq;
        ptr += sizeof (settings.seq);
        eeprom_read_block(&state_buf, (void *) ptr, sizeof (state_buf));
        ptr += sizeof (state_buf);
        settings.crc1 = eeprom_read_word((void *) ptr);
        ptr += sizeof (settings.crc1);
        settings.crc2 = eeprom_read_word((void *) ptr);
        ptr += sizeof (settings.crc2);

        if (settings.crc1 != crc16((void *) &state_buf, sizeof (state_buf))) {
            LOG("CRC1 incorrect\r\n");
            break;
        }
        uint16_t crc2_buf = _crc16_update(settings.crc1, (uint8_t)(settings.crc1 >> 8)); // crc2 = crc of state struct + crc1. 1st byte of CRC1
        crc2_buf = _crc16_update(crc2_buf, (uint8_t)settings.crc1); // and 2nd byte of CRC1
        if (crc2_buf != settings.crc2) {
            LOG("CRC2 incorrect\r\n");
            break;
        }
        // Since we are here, settings are valid, both CRCs are good
        state = state_buf;
        queue_num++;
    }
    sei();
}

void save_settings(eSaveMode savemode) {
    cli();
    LOG("Saving settings, mode = %d\r\n", savemode);
    if (savemode == SAVE_FULL || savemode == SAVE_CRC1)
        settings.crc1 = crc16((void *) &state, sizeof (state)); // Calculate crc16 of state struct
    if (settings.crc1 && (savemode == SAVE_FULL || savemode == SAVE_CRC2)) {
        settings.crc2 = _crc16_update(settings.crc1, (uint8_t) (settings.crc1 >> 8)); // crc2 = crc of state struct + crc1. 1st byte of CRC1
        settings.crc2 = _crc16_update(settings.crc2, (uint8_t) settings.crc1); // and 2nd byte of CRC1
    }
#ifdef VERBOSE_LOGS
    LOG("Settings size = %d, CRC1 = %X, CRC2 = %X.\r\n", sizeof (state) + sizeof (settings), settings.crc1, settings.crc2);
#endif
    // Save data to EEPROM
    settings.seq++;
    if (queue_num > EEPROM_ENTRIES) // Reset queue_num once we reach end of EEPROM
        queue_num = 0;
    uint8_t *ptr = (void *)(queue_num * EEPROM_ENTRY_SIZE); // Get location of next segment in EEPROM
    
#ifdef VERBOSE_LOGS
    LOG("Q = %d, PTR = %d\r\n", queue_num, ptr);
#endif
    
    eeprom_write_dword((void *) ptr, settings.seq);
    ptr += sizeof (settings.seq);
    eeprom_update_block((void *)&state, (void *) ptr, sizeof (state));
    ptr += sizeof (state);
    eeprom_write_word((void *) ptr, settings.crc1);
    ptr += sizeof (settings.crc1);
    eeprom_write_word((void *) ptr, settings.crc2);
    queue_num++;

    // Reset CRCs when save are complete
    if (savemode == SAVE_FULL || savemode == SAVE_CRC2) {
        settings.crc1 = 0;
        settings.crc2 = 0;
    }
    sei();
}
