#include <string.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "eeprom_action.h"
#include "memory.h"
#include "lookup.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <avr/pgmspace.h>
static void logPgm(LogFunc log, const char *msgFlash) {
    if (log) {
        char buf[64];
        strncpy_P(buf, msgFlash, sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';
        log(buf);
    }
}
#else
// compatibility stubs
#define logPgm(log, msg) if (log) log(msg)
#endif

static void writeByteToEEPROM(int *addr, uint8_t val, uint16_t *checksum) {
    eeprom_update_byte((uint8_t *)(*addr), val);
    *checksum += val;
    (*addr)++;
}

static void writeFixedFieldToEEPROM(int *addr, const char *src, int maxLen, uint16_t *checksum) {
    int i = 0;
    while (i < maxLen - 1 && src[i] != '\0') {
        writeByteToEEPROM(addr, src[i], checksum);
        i++;
    }
    writeByteToEEPROM(addr, '\0', checksum);
}

static void writeLookupFieldToEEPROM(int *addr, uint8_t tableId, uint8_t idx, uint16_t *checksum) {
    if (idx < 128) {
        writeByteToEEPROM(addr, idx, checksum);
    } else {
        // dynamic custom string
        writeByteToEEPROM(addr, 255, checksum);
        char buf[32];
        getLookupString(tableId, idx, buf, sizeof(buf));
        writeFixedFieldToEEPROM(addr, buf, sizeof(buf), checksum);
    }
}

static void readByteFromEEPROM(int *addr, uint8_t *val, uint16_t *checksum) {
    *val = eeprom_read_byte((const uint8_t *)(*addr));
    *checksum += *val;
    (*addr)++;
}

static void readStringFromEEPROM(int *addr, char *dest, int maxLen, uint16_t *checksum) {
    int i = 0;
    while (i < maxLen - 1) {
        uint8_t b;
        readByteFromEEPROM(addr, &b, checksum);
        dest[i] = b;
        if (b == 0) {
            return;
        }
        i++;
    }
    dest[maxLen - 1] = '\0';
    // consume remaining bytes in EEPROM if the string was longer than maxLen-1
    while (1) {
        uint8_t b;
        readByteFromEEPROM(addr, &b, checksum);
        if (b == 0) break;
    }
}

static void readLookupFieldFromEEPROM(int *addr, uint8_t tableId, uint8_t *idx, uint16_t *checksum) {
    uint8_t val;
    readByteFromEEPROM(addr, &val, checksum);
    if (val != 255) {
        *idx = val;
    } else {
        // dynamic custom string
        char buf[32];
        readStringFromEEPROM(addr, buf, sizeof(buf), checksum);
        *idx = registerLookup(tableId, buf);
    }
}

static int getRequiredSize(const Item *it) {
    int len_id = strnlen(it->id, 7);
    int len_nama = strnlen(it->nama, 19);
    int size = len_id + 1 + len_nama + 1; // strings + null-terminators
    
    // Kategori
    if (it->kategoriIdx < 128) {
        size += 1;
    } else {
        char buf[32];
        getLookupString(TABLE_KATEGORI, it->kategoriIdx, buf, sizeof(buf));
        size += 1 + strlen(buf) + 1;
    }

    // Lokasi
    if (it->lokasiIdx < 128) {
        size += 1;
    } else {
        char buf[32];
        getLookupString(TABLE_LOKASI, it->lokasiIdx, buf, sizeof(buf));
        size += 1 + strlen(buf) + 1;
    }

    // Pemilik
    if (it->pemilikIdx < 128) {
        size += 1;
    } else {
        char buf[32];
        getLookupString(TABLE_PEMILIK, it->pemilikIdx, buf, sizeof(buf));
        size += 1 + strlen(buf) + 1;
    }

    // PIC
    if (it->picIdx < 128) {
        size += 1;
    } else {
        char buf[32];
        getLookupString(TABLE_PIC, it->picIdx, buf, sizeof(buf));
        size += 1 + strlen(buf) + 1;
    }

    size += 4;
    return size;
}

void saveToEEPROM(List L, LogFunc log) {
    EEPROMHeader header;
    header.magic = MAGIC_NUMBER;
    header.checksum = 0;

    int addr = sizeof(EEPROMHeader);
    List curr = L;
    uint16_t savedCount = 0;

    while (curr != NULL) {
        int required = getRequiredSize(curr);
        if (addr + required > EEPROM_SIZE) {
            logPgm(log, PSTR("[ERROR] EEPROM penuh, tidak semua data tersimpan"));
            break;
        }

        writeFixedFieldToEEPROM(&addr, curr->id, sizeof(curr->id), &header.checksum);
        writeFixedFieldToEEPROM(&addr, curr->nama, sizeof(curr->nama), &header.checksum);
        writeLookupFieldToEEPROM(&addr, TABLE_KATEGORI, curr->kategoriIdx, &header.checksum);
        writeLookupFieldToEEPROM(&addr, TABLE_LOKASI, curr->lokasiIdx, &header.checksum);
        writeLookupFieldToEEPROM(&addr, TABLE_PEMILIK, curr->pemilikIdx, &header.checksum);
        writeLookupFieldToEEPROM(&addr, TABLE_PIC, curr->picIdx, &header.checksum);
        writeByteToEEPROM(&addr, curr->tersedia, &header.checksum);
        writeByteToEEPROM(&addr, curr->dipinjam, &header.checksum);
        writeByteToEEPROM(&addr, curr->rusak, &header.checksum);
        writeByteToEEPROM(&addr, curr->habis, &header.checksum);

        savedCount++;
        curr = curr->next;
    }

    header.itemCount = savedCount;

    // save header
    eeprom_update_byte((uint8_t *)0, header.magic);
    eeprom_update_byte((uint8_t *)1, (header.itemCount >> 8) & 0xFF);
    eeprom_update_byte((uint8_t *)2, header.itemCount & 0xFF);
    eeprom_update_byte((uint8_t *)3, (header.checksum >> 8) & 0xFF);
    eeprom_update_byte((uint8_t *)4, header.checksum & 0xFF);

    logPgm(log, PSTR("[MEMORY] Data berhasil disimpan"));
}

void loadFromEEPROM(List *L, LogFunc log) {
    EEPROMHeader header;
    Item data;
    int status = 0;

    header.magic = eeprom_read_byte((const uint8_t *)0);
    header.itemCount = ((uint16_t)eeprom_read_byte((const uint8_t *)1) << 8)
                     | eeprom_read_byte((const uint8_t *)2);
    header.checksum = ((uint16_t)eeprom_read_byte((const uint8_t *)3) << 8)
                    | eeprom_read_byte((const uint8_t *)4);

    if (header.magic != MAGIC_NUMBER) {
        logPgm(log, PSTR("[MEMORY] Data kosong atau corrupt"));
        *L = NULL;
        return;
    }

    if (header.itemCount > MAX_ITEMS || header.itemCount == 0) {
        logPgm(log, PSTR("[MEMORY] itemCount tidak valid"));
        *L = NULL;
        return;
    }

    int addr = sizeof(EEPROMHeader);
    uint16_t computedChecksum = 0;

    // clear dynamic lookup tables to start fresh
    initLookup();

    for (int i = 0; i < header.itemCount; i++) {
        memset(&data, 0, sizeof(Item));

        readStringFromEEPROM(&addr, data.id, sizeof(data.id), &computedChecksum);
        readStringFromEEPROM(&addr, data.nama, sizeof(data.nama), &computedChecksum);
        readLookupFieldFromEEPROM(&addr, TABLE_KATEGORI, &data.kategoriIdx, &computedChecksum);
        readLookupFieldFromEEPROM(&addr, TABLE_LOKASI, &data.lokasiIdx, &computedChecksum);
        readLookupFieldFromEEPROM(&addr, TABLE_PEMILIK, &data.pemilikIdx, &computedChecksum);
        readLookupFieldFromEEPROM(&addr, TABLE_PIC, &data.picIdx, &computedChecksum);
        readByteFromEEPROM(&addr, &data.tersedia, &computedChecksum);
        readByteFromEEPROM(&addr, &data.dipinjam, &computedChecksum);
        readByteFromEEPROM(&addr, &data.rusak, &computedChecksum);
        readByteFromEEPROM(&addr, &data.habis, &computedChecksum);

        data.next = NULL;
        insertSorted(L, data, &status);

        if (status != 0) {
            logPgm(log, PSTR("[MEMORY] Error loading item"));
        }
    }

    if (computedChecksum != header.checksum) {
        logPgm(log, PSTR("[MEMORY] PERINGATAN: Checksum tidak cocok!"));
    }

    logPgm(log, PSTR("[MEMORY] Data berhasil dimuat"));
}

void clearEEPROM(LogFunc log) {
    for (int i = 0; i < EEPROM_SIZE; i++) {
        eeprom_update_byte((uint8_t *)i, 0);
    }
    logPgm(log, PSTR("[MEMORY] Seluruh MEMORY sudah dihapus"));
}

void displayEEPROMInfo(LogFunc log) {
    if (!log) return;

    EEPROMHeader header;
    char msg[32];

    header.magic = eeprom_read_byte((const uint8_t *)0);
    header.itemCount = ((uint16_t)eeprom_read_byte((const uint8_t *)1) << 8)
                     | eeprom_read_byte((const uint8_t *)2);
    header.checksum = ((uint16_t)eeprom_read_byte((const uint8_t *)3) << 8)
                    | eeprom_read_byte((const uint8_t *)4);

    logPgm(log, PSTR("====== INFO MEMORY ======"));
    if (header.magic == MAGIC_NUMBER) {
        logPgm(log, PSTR("Status: VALID"));
    } else {
        logPgm(log, PSTR("Status: KOSONG/CORRUPT"));
    }
    
    logPgm(log, PSTR("Jumlah Item:"));
    itoa(header.itemCount, msg, 10);
    log(msg);
    
    logPgm(log, PSTR("Checksum:"));
    itoa(header.checksum, msg, 16);
    log(msg);
    logPgm(log, PSTR("========================"));
}