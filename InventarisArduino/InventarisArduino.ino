#include "data.h"
#include "memory.h"
#include "validation.h"
#include "inventory.h"
#include "eeprom_action.h"
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>
    
List inventaris = NULL;

volatile boolean needsSaveToEEPROM = false;
unsigned long lastEEPROMWriteTime = 0;
#define EEPROM_WRITE_INTERVAL 1000

void arduinoLog (const char *msg){
    Serial.println(msg);
}

void bacaTeks(char *buf, int maxLen){
    while(!Serial.available()){
    }
    int len = Serial.readBytesUntil('\n', buf, maxLen - 1);
    buf[len] = '\0';
    // Trim trailing whitespace
    while(len > 0 && (buf[len-1] == '\r' || buf[len-1] == ' ')){
        buf[--len] = '\0';
    }
}

void bacaAngka(char *prompt, int *angka){
    Serial.println(prompt);
    while(!Serial.available()){
    }
    *angka = Serial.parseInt();
    // Consume remaining newline
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
    }
}

void tampilMenu(){
    Serial.println();
    Serial.println(F("┌────────────────────────────────────────────────────────┐"));
    Serial.println(F("│                 SISTEM INVENTARIS LAB                  │"));
    Serial.println(F("├────────────────────────────────────────────────────────┤"));
    Serial.println(F("│  [1] Tambah Item            [7] Pinjam Item            │"));
    Serial.println(F("│  [2] Hapus Item             [8] Kembalikan Item        │"));
    Serial.println(F("│  [3] Cari Item              [9] Tandai Rusak           │"));
    Serial.println(F("│  [4] Tampilkan Semua        [10] Tandai Habis          │"));
    Serial.println(F("│  [5] Tambah Stok            [11] Ringkasan             │"));
    Serial.println(F("│  [6] Kurangi Stok           [0] Keluar                 │"));
    Serial.println(F("└────────────────────────────────────────────────────────┘"));
    Serial.println(F("Masukkan Menu:"));
}

void setup(){
    pinMode(LED_BUILTIN, OUTPUT);

    // Blink LED 3 times to confirm sketch is running
    for(int i = 0; i < 3; i++){
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
        digitalWrite(LED_BUILTIN, LOW);
        delay(200);
    }

    Serial.begin(9600);
    while(!Serial){
    }

    // Small delay for Serial to stabilize
    delay(100);

    Serial.println();
    Serial.println(F("[SISTEM] Memuat data dari EEPROM..."));

    loadFromEEPROM(&inventaris, arduinoLog);

    if (inventaris == NULL) {
        Serial.println(F("[SISTEM] EEPROM kosong. Memuat data awal default..."));
        int status;
        item dataDefault;

        // 1. ESP-01
        strcpy(dataDefault.id, "ESP-01");
        strcpy(dataDefault.nama, "ESP32_Micro_Board");
        strcpy(dataDefault.kategori, "Mikrokontroler");
        strcpy(dataDefault.lokasi, "Lab_EE");
        strcpy(dataDefault.pemilik, "Pak_Nana");
        strcpy(dataDefault.pic, "Ken");
        dataDefault.tersedia = 10;
        dataDefault.dipinjam = 0; dataDefault.rusak = 0; dataDefault.habis = 0;
        tambahItem(&inventaris, dataDefault, &status);

        // 2. RSP-04
        strcpy(dataDefault.id, "RSP-04");
        strcpy(dataDefault.nama, "Raspberry_Pi_4");
        strcpy(dataDefault.kategori, "SBC");
        strcpy(dataDefault.lokasi, "Lab_EE");
        strcpy(dataDefault.pemilik, "Pak_Nana");
        strcpy(dataDefault.pic, "Ken");
        dataDefault.tersedia = 5;
        dataDefault.dipinjam = 0; dataDefault.rusak = 0; dataDefault.habis = 0;
        tambahItem(&inventaris, dataDefault, &status);

        // 3. ARD-01
        strcpy(dataDefault.id, "ARD-01");
        strcpy(dataDefault.nama, "Arduino_Uno");
        strcpy(dataDefault.kategori, "Mikrokontroler");
        strcpy(dataDefault.lokasi, "Lab_EE");
        strcpy(dataDefault.pemilik, "Pak_Infall");
        strcpy(dataDefault.pic, "Ken");
        dataDefault.tersedia = 20;
        dataDefault.dipinjam = 0; dataDefault.rusak = 0; dataDefault.habis = 0;
        tambahItem(&inventaris, dataDefault, &status);

        // 4. LDR-02
        strcpy(dataDefault.id, "LDR-02");
        strcpy(dataDefault.nama, "Sensor_LDR");
        strcpy(dataDefault.kategori, "Sensor");
        strcpy(dataDefault.lokasi, "Lab_Fisika");
        strcpy(dataDefault.pemilik, "Pak_Budi");
        strcpy(dataDefault.pic, "Azka");
        dataDefault.tersedia = 50;
        dataDefault.dipinjam = 0; dataDefault.rusak = 0; dataDefault.habis = 0;
        tambahItem(&inventaris, dataDefault, &status);

        // 5. MOT-01
        strcpy(dataDefault.id, "MOT-01");
        strcpy(dataDefault.nama, "Servo_Motor_SG90");
        strcpy(dataDefault.kategori, "Aktuator");
        strcpy(dataDefault.lokasi, "Lab_Robo");
        strcpy(dataDefault.pemilik, "Pak_Yusuf");
        strcpy(dataDefault.pic, "Kuri");
        dataDefault.tersedia = 15;
        dataDefault.dipinjam = 0; dataDefault.rusak = 0; dataDefault.habis = 0;
        tambahItem(&inventaris, dataDefault, &status);

        // 6. LCD-01
        strcpy(dataDefault.id, "LCD-01");
        strcpy(dataDefault.nama, "I2C_LCD_16x2");
        strcpy(dataDefault.kategori, "Display");
        strcpy(dataDefault.lokasi, "Lab_EE");
        strcpy(dataDefault.pemilik, "Pak_Infall");
        strcpy(dataDefault.pic, "Kyrie");
        dataDefault.tersedia = 8;
        dataDefault.dipinjam = 0; dataDefault.rusak = 0; dataDefault.habis = 0;
        tambahItem(&inventaris, dataDefault, &status);

        // 7. MUL-01
        strcpy(dataDefault.id, "MUL-01");
        strcpy(dataDefault.nama, "Multimeter_Digital");
        strcpy(dataDefault.kategori, "Alat_Ukur");
        strcpy(dataDefault.lokasi, "Lab_EE");
        strcpy(dataDefault.pemilik, "Pak_Nana");
        strcpy(dataDefault.pic, "Kenneth");
        dataDefault.tersedia = 4;
        dataDefault.dipinjam = 0; dataDefault.rusak = 0; dataDefault.habis = 0;
        tambahItem(&inventaris, dataDefault, &status);

        // Simpan langsung ke EEPROM
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

    item data;
    List curr;

    int status;
    int jumlah;

    char id[8];

    switch(menu){

        case 1:
            Serial.println(F("ID:"));
            bacaTeks(data.id, sizeof(data.id));

            Serial.println(F("Nama:"));
            bacaTeks(data.nama, sizeof(data.nama));

            Serial.println(F("Kategori:"));
            bacaTeks(data.kategori, sizeof(data.kategori));

            Serial.println(F("Lokasi:"));
            bacaTeks(data.lokasi, sizeof(data.lokasi));

            Serial.println(F("Pemilik:"));
            bacaTeks(data.pemilik, sizeof(data.pemilik));

            Serial.println(F("PIC:"));
            bacaTeks(data.pic, sizeof(data.pic));
            Serial.println(F("Stok Awal:"));
            int tempStok;
            bacaAngka("", &tempStok);
            data.tersedia = tempStok;

            
            data.dipinjam = 0;
            data.rusak = 0;
            data.habis = 0;

            tambahItem(&inventaris,data,&status);

            if(status == 0) needsSaveToEEPROM = true;

            tampilStatus(status);

            break;

        case 2:

            Serial.println(F("ID Item:"));

            bacaTeks(id, sizeof(id));

            deleteItem(&inventaris,id,&status);

            if(status == 0) needsSaveToEEPROM = true;

            tampilStatus(status);

            break;

        case 3:

            Serial.println(F("ID Item:"));

            bacaTeks(id, sizeof(id));

            curr = inventaris;

            while(curr != NULL){

                if(strcmp(curr->id,id) == 0){

                    Serial.println();
                    Serial.println(F("┌──────────────────────────────┐"));
                    Serial.print(F("│ ID       : ")); Serial.println(curr->id);
                    Serial.print(F("│ Nama     : ")); Serial.println(curr->nama);
                    Serial.print(F("│ Kategori : ")); Serial.println(curr->kategori);
                    Serial.print(F("│ Lokasi   : ")); Serial.println(curr->lokasi);
                    Serial.print(F("│ Pemilik  : ")); Serial.println(curr->pemilik);
                    Serial.print(F("│ PIC      : ")); Serial.println(curr->pic);
                    Serial.println(F("├──────────────────────────────┤"));
                    Serial.print(F("│ Total    : ")); Serial.println(getStokTotal(curr));
                    Serial.print(F("│ Tersedia : ")); Serial.println(curr->tersedia);
                    Serial.print(F("│ Dipinjam : ")); Serial.println(curr->dipinjam);
                    Serial.print(F("│ Rusak    : ")); Serial.println(curr->rusak);
                    Serial.print(F("│ Habis    : ")); Serial.println(curr->habis);
                    Serial.println(F("└──────────────────────────────┘"));

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

            tambahStok(inventaris,id,jumlah,&status);

            if(status == 0) needsSaveToEEPROM = true;
            tampilStatus(status);

            break;

        case 6:

            Serial.println(F("ID Item:"));
            bacaTeks(id, sizeof(id));

            Serial.println(F("Jumlah:"));
            bacaAngka("", &jumlah);

            kurangiStok(inventaris,id,jumlah,&status);

            if(status == 0) needsSaveToEEPROM = true;
            tampilStatus(status);

            break;

        case 7:

            Serial.println(F("ID Item:"));
            bacaTeks(id, sizeof(id));

            Serial.println(F("Jumlah:"));
            bacaAngka("", &jumlah);

            pinjamItem(inventaris,id,jumlah,&status);

            if(status == 0) needsSaveToEEPROM = true;   

            tampilStatus(status);

            break;

        case 8:

            Serial.println(F("ID Item:"));
            bacaTeks(id, sizeof(id));

            Serial.println(F("Jumlah:"));
            bacaAngka("", &jumlah);

            kembalikanItem(inventaris,id,jumlah,&status);

            if(status == 0) needsSaveToEEPROM = true;

            tampilStatus(status);

            break;

        case 9:

            Serial.println(F("ID Item:"));
            bacaTeks(id, sizeof(id));

            Serial.println(F("Jumlah:"));
            bacaAngka("", &jumlah);

            tandaiRusak(inventaris,id,jumlah,&status);

            if(status == 0) needsSaveToEEPROM = true;
            
            tampilStatus(status);

            break;

        case 10:

            Serial.println(F("ID Item:"));
            bacaTeks(id, sizeof(id));

            Serial.println(F("Jumlah:"));
            bacaAngka("", &jumlah);

            tandaiHabis(inventaris,id,jumlah,&status);

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
            Serial.println(F("┌────────────────────────────────────────┐"));
            Serial.println(F("│          RINGKASAN INVENTARIS          │"));
            Serial.println(F("├────────────────────────────────────────┤"));
            Serial.print(F("│ Total Jenis Barang : ")); Serial.println(totalItem);
            Serial.print(F("│ Total Unit Fisik   : ")); Serial.println(totalStok);
            Serial.println(F("├────────────────────────────────────────┤"));
            // Tersedia
            {
                Serial.print(F("│ Tersedia : "));
                int fill = (totalStok > 0) ? (totalTersedia * 15 / totalStok) : 0;
                for (int i = 0; i < 15; i++) {
                    if (i < fill) Serial.print(F("■"));
                    else Serial.print(F(" "));
                }
                Serial.print(F("  ")); Serial.print(totalTersedia);
                Serial.print(F(" unit ("));
                if (totalStok > 0) Serial.print((totalTersedia * 100.0) / totalStok, 1);
                else Serial.print(0.0, 1);
                Serial.println(F("%)"));
            }
            // Dipinjam
            {
                Serial.print(F("│ Dipinjam : "));
                int fill = (totalStok > 0) ? (totalDipinjam * 15 / totalStok) : 0;
                for (int i = 0; i < 15; i++) {
                    if (i < fill) Serial.print(F("■"));
                    else Serial.print(F(" "));
                }
                Serial.print(F("  ")); Serial.print(totalDipinjam);
                Serial.print(F(" unit ("));
                if (totalStok > 0) Serial.print((totalDipinjam * 100.0) / totalStok, 1);
                else Serial.print(0.0, 1);
                Serial.println(F("%)"));
            }
            // Rusak
            {
                Serial.print(F("│ Rusak    : "));
                int fill = (totalStok > 0) ? (totalRusak * 15 / totalStok) : 0;
                for (int i = 0; i < 15; i++) {
                    if (i < fill) Serial.print(F("■"));
                    else Serial.print(F(" "));
                }
                Serial.print(F("  ")); Serial.print(totalRusak);
                Serial.print(F(" unit ("));
                if (totalStok > 0) Serial.print((totalRusak * 100.0) / totalStok, 1);
                else Serial.print(0.0, 1);
                Serial.println(F("%)"));
            }
            // Habis
            {
                Serial.print(F("│ Habis    : "));
                int fill = (totalStok > 0) ? (totalHabis * 15 / totalStok) : 0;
                for (int i = 0; i < 15; i++) {
                    if (i < fill) Serial.print(F("■"));
                    else Serial.print(F(" "));
                }
                Serial.print(F("  ")); Serial.print(totalHabis);
                Serial.print(F(" unit ("));
                if (totalStok > 0) Serial.print((totalHabis * 100.0) / totalStok, 1);
                else Serial.print(0.0, 1);
                Serial.println(F("%)"));
            }
            Serial.println(F("└────────────────────────────────────────┘"));

            break;
        }

        default:
            Serial.println(F("Menu tidak valid"));
    }

    checkEEPROMWrite();

    tampilMenu();
}