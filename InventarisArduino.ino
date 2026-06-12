#include "data.h"
#include "memory.h"
#include "validation.h"
#include "inventory.h"
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

List inventaris = NULL;

String bacaTeks(){
    while(!Serial.available()){
    }

    String s = Serial.readStringUntil('\n');
    s.trim();

    return s;
}

int bacaAngka(){
    while(!Serial.available()){
    }

    int x = Serial.parseInt();

    while(Serial.available()){
        Serial.read();
    }

    return x;
}

void tampilStatus(int status){
    switch(status){

        case 0:
            Serial.println("Operasi berhasil");
            break;

        case 1:
            Serial.println("ID tidak ditemukan");
            break;

        case 2:
            Serial.println("Jumlah tidak valid");
            break;

        case 3:
            Serial.println("Stok tidak mencukupi");
            break;

        case 4:
            Serial.println("ID sudah terdaftar");
            break;

        case 5:
            Serial.println("Memori penuh");
            break;

        case 6:
            Serial.println("Jumlah pengembalian melebihi jumlah dipinjam");
            break;

        case 7:
            Serial.println("Item masih dipinjam sehingga tidak bisa dihapus");
            break;
    }
}

void tampilMenu(){
    Serial.println();
    Serial.println("===== SISTEM INVENTARIS =====");
    Serial.println("1. Tambah Item");
    Serial.println("2. Hapus Item");
    Serial.println("3. Cari Item");
    Serial.println("4. Tampilkan Semua");
    Serial.println("5. Tambah Stok");
    Serial.println("6. Kurangi Stok");
    Serial.println("7. Pinjam Item");
    Serial.println("8. Kembalikan Item");
    Serial.println("9. Tandai Rusak");
    Serial.println("10. Tandai Habis");
    Serial.println("11. Ringkasan");
    Serial.println("=============================");
    Serial.println("Masukkan Menu:");
}

void setup(){
    Serial.begin(9600);

    while(!Serial){
    }

    tampilMenu();
}

void loop(){

    if(!Serial.available()){
        return;
    }

    int menu = bacaAngka();

    item data;
    List curr;

    int status;
    int jumlah;

    char id[15];

    switch(menu){

        case 1:

            Serial.println("ID:");
            bacaTeks().toCharArray(data.id,15);

            Serial.println("Nama:");
            bacaTeks().toCharArray(data.nama,50);

            Serial.println("Kategori:");
            bacaTeks().toCharArray(data.kategori,30);

            Serial.println("Lokasi:");
            bacaTeks().toCharArray(data.lokasi,30);

            Serial.println("Pemilik:");
            bacaTeks().toCharArray(data.pemilik,50);

            Serial.println("PIC:");
            bacaTeks().toCharArray(data.pic,50);

            Serial.println("Stok Awal:");
            data.stokTotal = bacaAngka();

            data.tersedia = data.stokTotal;
            data.dipinjam = 0;
            data.rusak = 0;
            data.habis = 0;

            tambahItem(&inventaris,data,&status);

            tampilStatus(status);

            break;

        case 2:

            Serial.println("ID Item:");

            bacaTeks().toCharArray(id,15);

            deleteItem(&inventaris,id,&status);

            tampilStatus(status);

            break;

        case 3:

            Serial.println("ID Item:");

            bacaTeks().toCharArray(id,15);

            curr = inventaris;

            while(curr != NULL){

                if(strcmp(curr->id,id) == 0){

                    Serial.println();
                    Serial.print("ID : ");
                    Serial.println(curr->id);

                    Serial.print("Nama : ");
                    Serial.println(curr->nama);

                    Serial.print("Kategori : ");
                    Serial.println(curr->kategori);

                    Serial.print("Lokasi : ");
                    Serial.println(curr->lokasi);

                    Serial.print("Pemilik : ");
                    Serial.println(curr->pemilik);

                    Serial.print("PIC : ");
                    Serial.println(curr->pic);

                    Serial.print("Total : ");
                    Serial.println(curr->stokTotal);

                    Serial.print("Tersedia : ");
                    Serial.println(curr->tersedia);

                    Serial.print("Dipinjam : ");
                    Serial.println(curr->dipinjam);

                    Serial.print("Rusak : ");
                    Serial.println(curr->rusak);

                    Serial.print("Habis : ");
                    Serial.println(curr->habis);

                    break;
                }

                curr = curr->next;
            }

            if(curr == NULL){
                Serial.println("ID tidak ditemukan");
            }

            break;

        case 4:

            curr = inventaris;

            Serial.println();
            Serial.println("ID | Nama | Total | Tersedia | Dipinjam | Rusak | Habis");

            while(curr != NULL){

                Serial.print(curr->id);
                Serial.print(" | ");

                Serial.print(curr->nama);
                Serial.print(" | ");

                Serial.print(curr->stokTotal);
                Serial.print(" | ");

                Serial.print(curr->tersedia);
                Serial.print(" | ");

                Serial.print(curr->dipinjam);
                Serial.print(" | ");

                Serial.print(curr->rusak);
                Serial.print(" | ");

                Serial.println(curr->habis);

                curr = curr->next;
            }

            break;

        case 5:

            Serial.println("ID Item:");
            bacaTeks().toCharArray(id,15);

            Serial.println("Jumlah:");
            jumlah = bacaAngka();

            tambahStok(inventaris,id,jumlah,&status);

            tampilStatus(status);

            break;

        case 6:

            Serial.println("ID Item:");
            bacaTeks().toCharArray(id,15);

            Serial.println("Jumlah:");
            jumlah = bacaAngka();

            kurangiStok(inventaris,id,jumlah,&status);

            tampilStatus(status);

            break;

        case 7:

            Serial.println("ID Item:");
            bacaTeks().toCharArray(id,15);

            Serial.println("Jumlah:");
            jumlah = bacaAngka();

            pinjamItem(inventaris,id,jumlah,&status);

            tampilStatus(status);

            break;

        case 8:

            Serial.println("ID Item:");
            bacaTeks().toCharArray(id,15);

            Serial.println("Jumlah:");
            jumlah = bacaAngka();

            kembalikanItem(inventaris,id,jumlah,&status);

            tampilStatus(status);

            break;

        case 9:

            Serial.println("ID Item:");
            bacaTeks().toCharArray(id,15);

            Serial.println("Jumlah:");
            jumlah = bacaAngka();

            tandaiRusak(inventaris,id,jumlah,&status);

            tampilStatus(status);

            break;

        case 10:

            Serial.println("ID Item:");
            bacaTeks().toCharArray(id,15);

            Serial.println("Jumlah:");
            jumlah = bacaAngka();

            tandaiHabis(inventaris,id,jumlah,&status);

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

                totalStok += curr->stokTotal;
                totalTersedia += curr->tersedia;
                totalDipinjam += curr->dipinjam;
                totalRusak += curr->rusak;
                totalHabis += curr->habis;

                curr = curr->next;
            }

            Serial.println();
            Serial.print("Jumlah Item : ");
            Serial.println(totalItem);

            Serial.print("Total Stok : ");
            Serial.println(totalStok);

            Serial.print("Total Tersedia : ");
            Serial.println(totalTersedia);

            Serial.print("Total Dipinjam : ");
            Serial.println(totalDipinjam);

            Serial.print("Total Rusak : ");
            Serial.println(totalRusak);

            Serial.print("Total Habis : ");
            Serial.println(totalHabis);

            break;
        }

        default:
            Serial.println("Menu tidak valid");
    }

    tampilMenu();
}