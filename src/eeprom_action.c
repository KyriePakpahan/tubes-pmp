#include <string.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "eeprom_action.h"
#include "memory.h"

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

static void writeInt16ToEEPROM(int *addr, int16_t val, uint16_t *checksum) {
    writeByteToEEPROM(addr, (val >> 8) & 0xFF, checksum);
    writeByteToEEPROM(addr, val & 0xFF, checksum);
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
    // Consume remaining bytes in EEPROM if the string was longer than maxLen-1
    while (1) {
        uint8_t b;
        readByteFromEEPROM(addr, &b, checksum);
        if (b == 0) break;
    }
}

static void readInt16FromEEPROM(int *addr, int16_t *val, uint16_t *checksum) {
    uint8_t hi, lo;
    readByteFromEEPROM(addr, &hi, checksum);
    readByteFromEEPROM(addr, &lo, checksum);
    *val = ((int16_t)hi << 8) | lo;
}

static int getRequiredSize(item *it) {
    int len_id = strnlen(it->id, 7);
    int len_nama = strnlen(it->nama, 19);
    int len_kategori = strnlen(it->kategori, 14);
    int len_lokasi = strnlen(it->lokasi, 7);
    int len_pemilik = strnlen(it->pemilik, 9);
    int len_pic = strnlen(it->pic, 7);
    return len_id + len_nama + len_kategori + len_lokasi + len_pemilik + len_pic + 6 + 8;
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
            if (log) log("[ERROR] EEPROM penuh, tidak semua data tersimpan");
            break;
        }

        writeFixedFieldToEEPROM(&addr, curr->id, sizeof(curr->id), &header.checksum);
        writeFixedFieldToEEPROM(&addr, curr->nama, sizeof(curr->nama), &header.checksum);
        writeFixedFieldToEEPROM(&addr, curr->kategori, sizeof(curr->kategori), &header.checksum);
        writeFixedFieldToEEPROM(&addr, curr->lokasi, sizeof(curr->lokasi), &header.checksum);
        writeFixedFieldToEEPROM(&addr, curr->pemilik, sizeof(curr->pemilik), &header.checksum);
        writeFixedFieldToEEPROM(&addr, curr->pic, sizeof(curr->pic), &header.checksum);
        writeInt16ToEEPROM(&addr, curr->tersedia, &header.checksum);
        writeInt16ToEEPROM(&addr, curr->dipinjam, &header.checksum);
        writeInt16ToEEPROM(&addr, curr->rusak, &header.checksum);
        writeInt16ToEEPROM(&addr, curr->habis, &header.checksum);

        savedCount++;
        curr = curr->next;
    }

    header.itemCount = savedCount;

    // Save header
    eeprom_update_byte((uint8_t *)0, header.magic);
    eeprom_update_byte((uint8_t *)1, (header.itemCount >> 8) & 0xFF);
    eeprom_update_byte((uint8_t *)2, header.itemCount & 0xFF);
    eeprom_update_byte((uint8_t *)3, (header.checksum >> 8) & 0xFF);
    eeprom_update_byte((uint8_t *)4, header.checksum & 0xFF);

    if (log) log("[MEMORY] Data berhasil disimpan");
}

void loadFromEEPROM(List *L, LogFunc log) {
    EEPROMHeader header;
    item data;
    int status = 0;

    header.magic = eeprom_read_byte((const uint8_t *)0);
    header.itemCount = ((uint16_t)eeprom_read_byte((const uint8_t *)1) << 8)
                     | eeprom_read_byte((const uint8_t *)2);
    header.checksum = ((uint16_t)eeprom_read_byte((const uint8_t *)3) << 8)
                    | eeprom_read_byte((const uint8_t *)4);

    if (header.magic != MAGIC_NUMBER) {
        if (log) log("[MEMORY] Data kosong atau corrupt");
        *L = NULL;
        return;
    }

    if (header.itemCount > MAX_ITEMS || header.itemCount == 0) {
        if (log) log("[MEMORY] itemCount tidak valid");
        *L = NULL;
        return;
    }

    int addr = sizeof(EEPROMHeader);
    uint16_t computedChecksum = 0;

    for (int i = 0; i < header.itemCount; i++) {
        memset(&data, 0, sizeof(item));

        readStringFromEEPROM(&addr, data.id, sizeof(data.id), &computedChecksum);
        readStringFromEEPROM(&addr, data.nama, sizeof(data.nama), &computedChecksum);
        readStringFromEEPROM(&addr, data.kategori, sizeof(data.kategori), &computedChecksum);
        readStringFromEEPROM(&addr, data.lokasi, sizeof(data.lokasi), &computedChecksum);
        readStringFromEEPROM(&addr, data.pemilik, sizeof(data.pemilik), &computedChecksum);
        readStringFromEEPROM(&addr, data.pic, sizeof(data.pic), &computedChecksum);
        readInt16FromEEPROM(&addr, &data.tersedia, &computedChecksum);
        readInt16FromEEPROM(&addr, &data.dipinjam, &computedChecksum);
        readInt16FromEEPROM(&addr, &data.rusak, &computedChecksum);
        readInt16FromEEPROM(&addr, &data.habis, &computedChecksum);

        data.next = NULL;
        insertLast(L, data, &status);

        if (status != 0 && log) {
            log("[MEMORY] Error loading item");
        }
    }

    if (computedChecksum != header.checksum) {
        if (log) log("[MEMORY] PERINGATAN: Checksum tidak cocok!");
    }

    if (log) log("[MEMORY] Data berhasil dimuat");
}

void clearEEPROM(LogFunc log) {
    for (int i = 0; i < EEPROM_SIZE; i++) {
        eeprom_update_byte((uint8_t *)i, 0);
    }
    if (log) log("[MEMORY] Seluruh MEMORY sudah dihapus");
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

    log("====== INFO MEMORY ======");
    if (header.magic == MAGIC_NUMBER) {
        log("Status: VALID");
    } else {
        log("Status: KOSONG/CORRUPT");
    }
    
    log("Jumlah Item:");
    itoa(header.itemCount, msg, 10);
    log(msg);
    
    log("Checksum:");
    itoa(header.checksum, msg, 16);
    log(msg);
    log("========================");
}