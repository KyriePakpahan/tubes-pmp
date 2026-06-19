#include "data.h"
#include "memory.h"
#include "validation.h"
#include "inventory.h"
#include "eeprom_action.h"
#include "lookup.h"
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>
#include <avr/eeprom.h>
    
List inventaris = NULL;

volatile boolean needsSaveToEEPROM = false;
unsigned long lastEEPROMWriteTime = 0;
#define EEPROM_WRITE_INTERVAL 1000

void arduinoLog(const char *msg){
    Serial.println(msg);
}

void bacaTeks(char *buf, int maxLen){
    while(!Serial.available()){
    }
    int len = Serial.readBytesUntil('\n', buf, maxLen - 1);
    buf[len] = '\0';

    while(len > 0 && (buf[len-1] == '\r' || buf[len-1] == ' ')){
        buf[--len] = '\0';
    }
}

void bacaAngka(const char *prompt, int *angka){
    Serial.println(prompt);
    while(!Serial.available()){
    }
    *angka = Serial.parseInt();

    char discard[2];
    Serial.readBytesUntil('\n', discard, sizeof(discard));
}

void checkEEPROMWrite(){
    if (needsSaveToEEPROM && (millis() - lastEEPROMWriteTime >= EEPROM_WRITE_INTERVAL)){
        Serial.println(F("[MEMORY] Menyimpan data..."));
        saveToEEPROM(inventaris, arduinoLog);
        needsSaveToEEPROM = false;
        lastEEPROMWriteTime = millis();
        Serial.println(F("[MEMORY] Data berhasil disimpan."));
    }
}

extern unsigned int __heap_start;
extern void *__brkval;

int getFreeRAM() {
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void printPaddedLine(const __FlashStringHelper *label, const char *value) {
    char labelBuf[32];
    strcpy_P(labelBuf, (PGM_P)label);
    int lenL = strlen(labelBuf);
    int lenV = strlen(value);
    
    Serial.print(F("│  "));
    Serial.print(labelBuf);
    Serial.print(value);
    
    int pad = 54 - (lenL + lenV);
    for (int i = 0; i < pad; i++) {
        Serial.print(' ');
    }
    Serial.println(F("│"));
}

void printPaddedLine(const __FlashStringHelper *label, long value, const char *unit) {
    char valStr[32];
    ltoa(value, valStr, 10);
    if (unit) {
        strcat(valStr, " ");
        strcat(valStr, unit);
    }
    printPaddedLine(label, valStr);
}

void printPaddedLine(const __FlashStringHelper *label, double value, int decimalPlaces, const char *unit) {
    char valStr[32];
    dtostrf(value, 4, decimalPlaces, valStr);
    char *p = valStr;
    while (*p == ' ') p++;
    if (unit) {
        strcat(p, unit);
    }
    printPaddedLine(label, p);
}

void printPaddedBarLine(const __FlashStringHelper *label, int value, int total, const char *unit) {
    char labelBuf[32];
    strcpy_P(labelBuf, (PGM_P)label);
    int lenL = strlen(labelBuf);
    
    Serial.print(F("│  "));
    Serial.print(labelBuf);
    
    Serial.print('[');
    int fill = (total > 0) ? ((long)value * 15 / total) : 0;
    for (int i = 0; i < 15; i++) {
        if (i < fill) {
            Serial.print(F("■"));
        } else {
            Serial.print(' ');
        }
    }
    Serial.print(F("]  "));
    
    char valStr[32];
    if (total > 0) {
        double pct = (value * 100.0) / total;
        char pctStr[16];
        dtostrf(pct, 4, 1, pctStr);
        char *p = pctStr;
        while (*p == ' ') p++;
        sprintf(valStr, "%d %s (%s%%)", value, unit, p);
    } else {
        sprintf(valStr, "%d %s (0.0%%)", value, unit);
    }
    Serial.print(valStr);
    
    int lenV = strlen(valStr);
    int pad = 35 - lenL - lenV;
    for (int i = 0; i < pad; i++) {
        Serial.print(' ');
    }
    Serial.println(F("│"));
}

void tampilMemory() {
    Serial.println();
    Serial.println(F("┌────────────────────────────────────────────────────────┐"));
    Serial.println(F("│               MEMORY USAGE TRACKER                     │"));
    Serial.println(F("├────────────────────────────────────────────────────────┤"));

    int freeRAM = getFreeRAM();
    Serial.println(F("│  [ SRAM - 2048 bytes ]                                 │"));
    printPaddedLine(F("Free RAM        : "), freeRAM, "bytes");
    printPaddedLine(F("Used RAM        : "), 2048 - freeRAM, "bytes");
    printPaddedLine(F("Usage           : "), ((2048 - freeRAM) * 100.0) / 2048.0, 1, "%");

    // RAM bar
    Serial.print(F("│  ["));
    int ramFill = ((2048 - freeRAM) * 20) / 2048;
    for (int i = 0; i < 20; i++) {
        if (i < ramFill) Serial.print(F("█"));
        else Serial.print(F("░"));
    }
    Serial.print(F("]  "));
    char valStr[32];
    sprintf(valStr, "%d/%d Bytes", 2048 - freeRAM, 2048);
    Serial.print(valStr);
    int pad = 30 - strlen(valStr);
    for (int i = 0; i < pad; i++) {
        Serial.print(' ');
    }
    Serial.println(F("│"));

    Serial.println(F("├────────────────────────────────────────────────────────┤"));

    // read EEPROM header to get item count
    uint8_t magic = eeprom_read_byte((const uint8_t *)0);
    uint16_t itemCount = ((uint16_t)eeprom_read_byte((const uint8_t *)1) << 8)
                       | eeprom_read_byte((const uint8_t *)2);

    int eepromUsed = 0;
    if (magic == MAGIC_NUMBER && itemCount > 0 && itemCount <= MAX_ITEMS) {
        int addr = sizeof(EEPROMHeader);
        for (uint16_t i = 0; i < itemCount; i++) {
            while (eeprom_read_byte((const uint8_t *)addr) != 0 && addr < EEPROM_SIZE) addr++;
            addr++; // skip null
            
            while (eeprom_read_byte((const uint8_t *)addr) != 0 && addr < EEPROM_SIZE) addr++;
            addr++; // skip null
            
            for (int f = 0; f < 4; f++) {
                uint8_t val = eeprom_read_byte((const uint8_t *)addr);
                addr++;
                if (val == 255) {
                    while (eeprom_read_byte((const uint8_t *)addr) != 0 && addr < EEPROM_SIZE) addr++;
                    addr++; // skip null
                }
            }
            addr += 4;
        }
        eepromUsed = addr;
    } else {
        eepromUsed = sizeof(EEPROMHeader);  // only header
        itemCount = 0;
    }

    int eepromFree = EEPROM_SIZE - eepromUsed;

    Serial.println(F("│  [ EEPROM - 1024 bytes ]                               │"));
    printPaddedLine(F("Items Stored    : "), itemCount, NULL);
    printPaddedLine(F("Used EEPROM     : "), eepromUsed, "bytes");
    printPaddedLine(F("Free EEPROM     : "), eepromFree, "bytes");
    printPaddedLine(F("Usage           : "), (eepromUsed * 100.0) / EEPROM_SIZE, 1, "%");

    // EEPROM bar
    Serial.print(F("│  ["));
    int eeFill = (eepromUsed * 20) / EEPROM_SIZE;
    for (int i = 0; i < 20; i++) {
        if (i < eeFill) Serial.print(F("█"));
        else Serial.print(F("░"));
    }
    Serial.print(F("]  "));
    sprintf(valStr, "%d/%d Bytes", eepromUsed, EEPROM_SIZE);
    Serial.print(valStr);
    pad = 30 - strlen(valStr);
    for (int i = 0; i < pad; i++) {
        Serial.print(' ');
    }
    Serial.println(F("│"));

    // estimate max items
    if (itemCount > 0) {
        int avgPerItem = (eepromUsed - (int)sizeof(EEPROMHeader)) / itemCount;
        if (avgPerItem > 0) {
            int maxEstimate = (EEPROM_SIZE - (int)sizeof(EEPROMHeader)) / avgPerItem;
            char estStr[32];
            sprintf(estStr, "~%d (avg %d bytes/item)", maxEstimate, avgPerItem);
            printPaddedLine(F("Est. Max Items  : "), estStr);
        }
    }

    Serial.println(F("├────────────────────────────────────────────────────────┤"));

    int nodeCount = 0;
    List curr = inventaris;
    while (curr != NULL) {
        nodeCount++;
        curr = curr->next;
    }
    int nodeSize = (int)sizeof(Item);
    int heapUsed = nodeCount * nodeSize;

    Serial.println(F("│  [ HEAP - Linked List ]                                │"));
    printPaddedLine(F("Nodes Allocated : "), nodeCount, NULL);
    printPaddedLine(F("Node Size       : "), nodeSize, "bytes");
    printPaddedLine(F("Heap Used       : "), heapUsed, "bytes");

    Serial.println(F("├────────────────────────────────────────────────────────┤"));

    // diagnostic Section
    int needed = nodeSize + 200;  
    Serial.println(F("│  [ DIAGNOSTIK ]                                        │"));
    
    char neededStr[32];
    sprintf(neededStr, "%d bytes (node+stack)", needed);
    printPaddedLine(F("RAM utk item baru : "), neededStr);
    
    char statusStr[32];
    if (freeRAM >= needed) {
        sprintf(statusStr, "Ya (~%d item lagi)", (freeRAM - 200) / nodeSize);
    } else {
        sprintf(statusStr, "Tidak (kurang %d bytes)", needed - freeRAM);
    }
    printPaddedLine(F("Bisa tambah item  : "), statusStr);

    Serial.println(F("└────────────────────────────────────────────────────────┘"));
}

void tampilStatus(int status){
    switch(status){
        case 0:
            Serial.println(F("[SUKSES] Operasi berhasil"));
            break;
        case 1:
            Serial.println(F("[ERROR] ID tidak ditemukan"));
            break;
        case 2:
            Serial.println(F("[ERROR] Jumlah tidak valid"));
            break;
        case 3:
            Serial.println(F("[ERROR] Stok tidak mencukupi"));
            break;
        case 4:
            Serial.println(F("[ERROR] ID sudah terdaftar"));
            break;
        case 5:
            Serial.println(F("[ERROR] Memori penuh"));
            break;
        case 6:
            Serial.println(F("[ERROR] Jumlah pengembalian melebihi jumlah dipinjam"));
            break;
        case 7:
            Serial.println(F("[ERROR] Item masih dipinjam sehingga tidak bisa dihapus"));
            break;
        case 8:
            Serial.println(F("[ERROR] Nama barang sudah terdaftar"));
            break;
    }
}

void tampilMenu(){
    Serial.println();
    Serial.println(F("┌────────────────────────────────────────────────────────┐"));
    Serial.println(F("│                 SISTEM INVENTARIS LAB                  │"));
    Serial.println(F("├────────────────────────────────────────────────────────┤"));
    Serial.println(F("│  [1] Tambah Item            [7]  Pinjam Item           │"));
    Serial.println(F("│  [2] Hapus Item             [8]  Kembalikan Item       │"));
    Serial.println(F("│  [3] Cari Item              [9]  Tandai Rusak          │"));
    Serial.println(F("│  [4] Tampilkan Semua        [10] Tandai Habis          │"));
    Serial.println(F("│  [5] Tambah Stok            [11] Ringkasan             │"));
    Serial.println(F("│  [6] Kurangi Stok           [12] Memory Tracker        │"));
    Serial.println(F("│                             [0]  Keluar                │"));
    Serial.println(F("└────────────────────────────────────────────────────────┘"));
    Serial.println(F("Masukkan Menu:"));
}

uint8_t inputLookup(uint8_t tableId, const char *label) {
    Serial.print(F("Pilih "));
    Serial.print(label);
    Serial.println(F(":"));
    
    uint8_t staticCount = 0;
    if (tableId == TABLE_KATEGORI) staticCount = 12;
    else if (tableId == TABLE_LOKASI) staticCount = 3;
    else if (tableId == TABLE_PEMILIK) staticCount = 5;
    else if (tableId == TABLE_PIC) staticCount = 5;

    char buf[32];
    for (uint8_t i = 0; i < staticCount; i++) {
        getLookupString(tableId, i, buf, sizeof(buf));
        Serial.print(F("  ["));
        Serial.print(i);
        Serial.print(F("] "));
        Serial.print(buf);
        if ((i + 1) % 3 == 0 || i == staticCount - 1) {
            Serial.println();
        }
    }
    Serial.print(F("  ["));
    Serial.print(staticCount);
    Serial.println(F("] Lainnya"));
    
    int pilihan;
    bacaAngka("Pilih nomor:", &pilihan);
    
    if (pilihan >= 0 && pilihan < staticCount) {
        return pilihan;
    } else if (pilihan == staticCount) {
        Serial.print(F("Masukkan "));
        Serial.print(label);
        Serial.println(F(" Baru:"));
        bacaTeks(buf, sizeof(buf));
        uint8_t idx = registerLookup(tableId, buf);
        if (idx == LOOKUP_INVALID) {
            Serial.println(F("[ERROR] Gagal registrasi kustom (penuh/memori)"));
            return 0; 
        }
        return idx;
    } else {
        Serial.println(F("Pilihan tidak valid, menggunakan default [0]"));
        return 0;
    }
}

// SRAM-efficient default loader 
void addDefaultItem_P(const char *nama_P, const char *kat_P, const char *lok_P, const char *pem_P, const char *pic_P, uint8_t tersedia) {
    Item it;
    it.id[0] = '\0';
    strcpy_P(it.nama, nama_P);
    
    char buf[32];
    strcpy_P(buf, kat_P);
    it.kategoriIdx = registerLookup(TABLE_KATEGORI, buf);
    
    strcpy_P(buf, lok_P);
    it.lokasiIdx = registerLookup(TABLE_LOKASI, buf);
    
    strcpy_P(buf, pem_P);
    it.pemilikIdx = registerLookup(TABLE_PEMILIK, buf);
    
    strcpy_P(buf, pic_P);
    it.picIdx = registerLookup(TABLE_PIC, buf);
    
    it.tersedia = tersedia;
    it.dipinjam = 0; it.rusak = 0; it.habis = 0;
    
    int status;
    tambahItem(&inventaris, &it, &status);
}

void setup(){
    pinMode(LED_BUILTIN, OUTPUT);

    for(int i = 0; i < 3; i++){
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
        digitalWrite(LED_BUILTIN, LOW);
        delay(200);
    }

    Serial.begin(9600);
    while(!Serial){
    }
    delay(100);

    Serial.println();
    Serial.println(F("[SISTEM] Memuat data dari EEPROM..."));

    initLookup();
    loadFromEEPROM(&inventaris, arduinoLog);

    if (inventaris == NULL) {
        Serial.println(F("[SISTEM] EEPROM kosong. Memuat data awal default..."));
        
        // load default database using PROGMEM strings 
        addDefaultItem_P(PSTR("ESP32 Micro Board"), PSTR("Mikrokontroler"), PSTR("Lab EE"), PSTR("Pak Nana"), PSTR("Ken"), 10);
        addDefaultItem_P(PSTR("Raspberry Pi 4"), PSTR("SBC"), PSTR("Lab EE"), PSTR("Pak Nana"), PSTR("Ken"), 5);
        addDefaultItem_P(PSTR("Arduino Uno"), PSTR("Mikrokontroler"), PSTR("Lab EE"), PSTR("Pak Mervin"), PSTR("Ken"), 20);
        addDefaultItem_P(PSTR("Sensor LDR"), PSTR("Sensor"), PSTR("Lab Fisika"), PSTR("Pak Budi"), PSTR("Azka"), 50);
        addDefaultItem_P(PSTR("Servo Motor SG90"), PSTR("Aktuator"), PSTR("Lab Robo"), PSTR("Pak Mervin"), PSTR("Kuri"), 15);
        addDefaultItem_P(PSTR("I2C LCD 16x2"), PSTR("Display"), PSTR("Lab EE"), PSTR("Pak Mervin"), PSTR("Kyrie"), 8);
        addDefaultItem_P(PSTR("Multimeter Digital"), PSTR("Alat Ukur"), PSTR("Lab EE"), PSTR("Pak Nana"), PSTR("Kenneth"), 4);
        addDefaultItem_P(PSTR("Trans BC547"), PSTR("Diskrit"), PSTR("Lab EE"), PSTR("Pak Nana"), PSTR("Kenneth"), 35);
        addDefaultItem_P(PSTR("Push Button"), PSTR("Saklar"), PSTR("Lab Robo"), PSTR("Pak Yusuf"), PSTR("Kuri"), 60);
        addDefaultItem_P(PSTR("NE555 Timer"), PSTR("IC"), PSTR("Lab EE"), PSTR("Pak Infal"), PSTR("Kyrie"), 12);

        saveToEEPROM(inventaris, arduinoLog);
        Serial.println(F("[SISTEM] Data awal berhasil ditulis ke EEPROM."));
    } else {
        Serial.println(F("[SISTEM] Data inventaris berhasil dimuat dari Memori."));
    }

    needsSaveToEEPROM = false;
    lastEEPROMWriteTime = millis();

    tampilMenu();
}

void loop(){
    if(!Serial.available()){
        checkEEPROMWrite();
        return;
    }

    int menu = Serial.parseInt();
    Serial.readStringUntil('\n');   

    Item data;
    List curr;

    int status;
    int jumlah;
    char id[8];

    switch(menu){
        case 0:
            Serial.println();
            Serial.println(F("┌────────────────────────────────────────────────────────┐"));
            Serial.println(F("│                  SISTEM DINONAKTIFKAN                  │"));
            Serial.println(F("├────────────────────────────────────────────────────────┤"));
            Serial.println(F("│         Terima kasih! Program telah berhenti.          │"));
            Serial.println(F("│         Tekan tombol RESET pada board Arduino          │"));
            Serial.println(F("│            untuk menyalakan kembali sistem.            │"));
            Serial.println(F("└────────────────────────────────────────────────────────┘"));
            while(true) {
                delay(1000);
            }
            break;

        case 1:
            Serial.println(F("Nama:"));
            bacaTeks(data.nama, sizeof(data.nama));

            data.kategoriIdx = inputLookup(TABLE_KATEGORI, "Kategori");
            data.lokasiIdx = inputLookup(TABLE_LOKASI, "Lokasi");
            data.pemilikIdx = inputLookup(TABLE_PEMILIK, "Pemilik");
            data.picIdx = inputLookup(TABLE_PIC, "PIC");

            Serial.println(F("Stok Awal:"));
            int tempStok;
            bacaAngka("", &tempStok);
            if (tempStok <= 0 || tempStok > 255) {
                Serial.println(F("[ERROR] Stok melebihi kapasitas, harus 1-255!"));
                break;
            }
            data.tersedia = (uint8_t)tempStok;
            data.dipinjam = 0;
            data.rusak = 0;
            data.habis = 0;

            tambahItem(&inventaris, &data, &status);
            if(status == 0) {
                needsSaveToEEPROM = true;
                Serial.print(F("[SUKSES] Item ditambahkan dengan ID: "));
                Serial.println(data.id);
            } else {
                tampilStatus(status);
            }
            break;

        case 2:
            Serial.println(F("ID Item:"));
            bacaTeks(id, sizeof(id));
            deleteItem(&inventaris, id, &status);
            if(status == 0) needsSaveToEEPROM = true;
            tampilStatus(status);
            break;

        case 3:
            Serial.println(F("ID Item:"));
            bacaTeks(id, sizeof(id));
            curr = inventaris;

            while(curr != NULL){
                if(strcmp(curr->id, id) == 0){
                    char kat[24], lok[16], pem[16], pic[16];
                    getLookupString(TABLE_KATEGORI, curr->kategoriIdx, kat, sizeof(kat));
                    getLookupString(TABLE_LOKASI, curr->lokasiIdx, lok, sizeof(lok));
                    getLookupString(TABLE_PEMILIK, curr->pemilikIdx, pem, sizeof(pem));
                    getLookupString(TABLE_PIC, curr->picIdx, pic, sizeof(pic));

                    Serial.println();
                    Serial.println(F("┌────────────────────────────────────────────────────────┐"));
                    Serial.println(F("│                     INFORMASI ITEM                     │"));
                    Serial.println(F("├────────────────────────────────────────────────────────┤"));
                    printPaddedLine(F("ID          : "), curr->id);
                    printPaddedLine(F("Nama        : "), curr->nama);
                    printPaddedLine(F("Kategori    : "), kat);
                    printPaddedLine(F("Lokasi      : "), lok);
                    printPaddedLine(F("Pemilik     : "), pem);
                    printPaddedLine(F("PIC         : "), pic);
                    Serial.println(F("├────────────────────────────────────────────────────────┤"));
                    printPaddedLine(F("Total       : "), getStokTotal(curr), "unit");
                    printPaddedLine(F("Tersedia    : "), curr->tersedia, "unit");
                    printPaddedLine(F("Dipinjam    : "), curr->dipinjam, "unit");
                    printPaddedLine(F("Rusak       : "), curr->rusak, "unit");
                    printPaddedLine(F("Habis       : "), curr->habis, "unit");
                    Serial.println(F("└────────────────────────────────────────────────────────┘"));
                    break;
                }
                curr = curr->next;
            }

            if(curr == NULL){
                Serial.println(F("ID tidak ditemukan"));
            }
            break;

        case 4:
            curr = inventaris;
            Serial.println();
            Serial.println(F("┌──────────┬──────────────────────┬──────────┬──────────┬──────────┬──────────┬──────────┐"));
            Serial.println(F("│ ID       │ Nama Barang          │ Total    │ Tersedia │ Dipinjam │ Rusak    │ Habis    │"));
            Serial.println(F("├──────────┼──────────────────────┼──────────┼──────────┼──────────┼──────────┼──────────┤"));

            while(curr != NULL){
                char buf[120];
                snprintf(buf, sizeof(buf), "│ %-8s │ %-20s │ %8d │ %8d │ %8d │ %8d │ %8d │",
                         curr->id, curr->nama, getStokTotal(curr), curr->tersedia,
                         curr->dipinjam, curr->rusak, curr->habis);
                Serial.println(buf);
                curr = curr->next;
            }
            Serial.println(F("└──────────┴──────────────────────┴──────────┴──────────┴──────────┴──────────┴──────────┘"));
            break;

        case 5:
            Serial.println(F("ID Item:"));
            bacaTeks(id, sizeof(id));
            Serial.println(F("Jumlah:"));
            bacaAngka("", &jumlah);
            tambahStok(inventaris, id, jumlah, &status);
            if(status == 0) needsSaveToEEPROM = true;
            tampilStatus(status);
            break;

        case 6:
            Serial.println(F("ID Item:"));
            bacaTeks(id, sizeof(id));
            Serial.println(F("Jumlah:"));
            bacaAngka("", &jumlah);
            kurangiStok(inventaris, id, jumlah, &status);
            if(status == 0) needsSaveToEEPROM = true;
            tampilStatus(status);
            break;

        case 7:
            Serial.println(F("ID Item:"));
            bacaTeks(id, sizeof(id));
            Serial.println(F("Jumlah:"));
            bacaAngka("", &jumlah);
            pinjamItem(inventaris, id, jumlah, &status);
            if(status == 0) needsSaveToEEPROM = true;   
            tampilStatus(status);
            break;

        case 8:
            Serial.println(F("ID Item:"));
            bacaTeks(id, sizeof(id));
            Serial.println(F("Jumlah:"));
            bacaAngka("", &jumlah);
            kembalikanItem(inventaris, id, jumlah, &status);
            if(status == 0) needsSaveToEEPROM = true;
            tampilStatus(status);
            break;

        case 9:
            Serial.println(F("ID Item:"));
            bacaTeks(id, sizeof(id));
            Serial.println(F("Jumlah:"));
            bacaAngka("", &jumlah);
            tandaiRusak(inventaris, id, jumlah, &status);
            if(status == 0) needsSaveToEEPROM = true;
            tampilStatus(status);
            break;

        case 10:
            Serial.println(F("ID Item:"));
            bacaTeks(id, sizeof(id));
            Serial.println(F("Jumlah:"));
            bacaAngka("", &jumlah);
            tandaiHabis(inventaris, id, jumlah, &status);
            if(status == 0) needsSaveToEEPROM = true;
            tampilStatus(status);
            break;

        case 11:
        {
            int totalItem = 0;
            int totalStok = 0;
            int totalTersedia = 0;
            int totalDipinjam = 0;
            int totalRusak = 0;
            int totalHabis = 0;

            curr = inventaris;
            while(curr != NULL){
                totalItem++;
                totalStok += getStokTotal(curr);
                totalTersedia += curr->tersedia;
                totalDipinjam += curr->dipinjam;
                totalRusak += curr->rusak;
                totalHabis += curr->habis;
                curr = curr->next;
            }

            Serial.println();
            Serial.println(F("┌────────────────────────────────────────────────────────┐"));
            Serial.println(F("│                  RINGKASAN INVENTARIS                  │"));
            Serial.println(F("├────────────────────────────────────────────────────────┤"));
            printPaddedLine(F("Total Jenis Barang : "), totalItem, NULL);
            printPaddedLine(F("Total Unit Fisik   : "), totalStok, NULL);
            Serial.println(F("├────────────────────────────────────────────────────────┤"));
            printPaddedBarLine(F("Tersedia : "), totalTersedia, totalStok, "unit");
            printPaddedBarLine(F("Dipinjam : "), totalDipinjam, totalStok, "unit");
            printPaddedBarLine(F("Rusak    : "), totalRusak, totalStok, "unit");
            printPaddedBarLine(F("Habis    : "), totalHabis, totalStok, "unit");
            Serial.println(F("└────────────────────────────────────────────────────────┘"));
            break;
        }

        case 12:
            tampilMemory();
            break;

        default:
            Serial.println(F("Menu tidak valid"));
    }

    checkEEPROMWrite();
    tampilMenu();
}