#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/data.h"
#include "../include/inventory.h"
#include "../include/memory.h"
#include "../include/validation.h"

const char* getStatusText(int status) {
    switch(status) {
        case 0: return "Operasi berhasil";
        case 1: return "ID tidak ditemukan";
        case 2: return "Jumlah tidak valid";
        case 3: return "Stok tidak mencukupi";
        case 4: return "ID sudah terdaftar";
        case 5: return "Memori penuh";
        case 6: return "Pengembalian melebihi pinjaman";
        case 7: return "Item masih dipinjam";
        default: return "Status tidak diketahui";
    }
}

int main() {
    FILE *fin = fopen("test/input/testcase.txt", "r");
    FILE *fout = fopen("test/output/result.txt", "w");

    if (fin == NULL || fout == NULL) {
        printf("Error: Gagal membuka file testcase atau output.\n");
        return 1;
    }

    List inventaris = NULL;
    char command[20];
    int status;

    fprintf(fout, "=================================================\n");
    fprintf(fout, "        AUTOMATED TESTBENCH - INVENTARIS\n");
    fprintf(fout, "=================================================\n\n");

    while (fscanf(fin, "%s", command) != EOF) {
        // 1. TAMBAH ITEM
        if (strcmp(command, "ADD") == 0) {
            item data;
            fscanf(fin, "%s %s %s %s %s %s %d", 
                   data.id, data.nama, data.kategori, 
                   data.lokasi, data.pemilik, data.pic, &data.stokTotal);
            
            data.tersedia = data.stokTotal;
            data.dipinjam = 0;
            data.rusak = 0;
            data.habis = 0;
            
            tambahItem(&inventaris, data, &status);
            fprintf(fout, "[TEST: ADD] Menambahkan ID: %s | Nama: %s | Stok: %d\n", data.id, data.nama, data.stokTotal);
            fprintf(fout, "   -> HASIL: %s\n\n", getStatusText(status));
        }
        // 2. HAPUS ITEM
        else if (strcmp(command, "DELETE") == 0) {
            char id[15];
            fscanf(fin, "%s", id);
            
            deleteItem(&inventaris, id, &status);
            fprintf(fout, "[TEST: DELETE] Menghapus ID %s dari sistem\n", id);
            fprintf(fout, "   -> HASIL: %s\n\n", getStatusText(status));
        }
        // 3. CARI ITEM
        else if (strcmp(command, "FIND") == 0) {
            char id[15];
            fscanf(fin, "%s", id);
            fprintf(fout, "[TEST: FIND] Mencari ID %s dalam sistem\n", id);
            
            List curr = inventaris;
            int found = 0;
            while(curr != NULL){
                if(strcmp(curr->id, id) == 0){
                    fprintf(fout, "   -> DITEMUKAN: %s | Tersedia: %d | Dipinjam: %d\n\n", curr->nama, curr->tersedia, curr->dipinjam);
                    found = 1;
                    break;
                }
                curr = curr->next;
            }
            if(!found){
                fprintf(fout, "   -> HASIL: ID tidak ditemukan\n\n");
            }
        }
        // 4. TAMPILKAN SEMUA ITEM
        else if (strcmp(command, "DISPLAY_ALL") == 0) {
            fprintf(fout, "[TEST: DISPLAY_ALL] Menampilkan seluruh data inventaris\n");
            if (inventaris == NULL) {
                fprintf(fout, "   -> HASIL: Data inventaris kosong.\n\n");
            } else {
                fprintf(fout, "------------------------------------------------------------------------------------\n");
                fprintf(fout, "%-12s %-20s %-8s %-10s %-10s %-8s %-8s\n",
                       "ID", "Nama", "Total", "Tersedia", "Dipinjam", "Rusak", "Habis");
                fprintf(fout, "------------------------------------------------------------------------------------\n");
                List curr = inventaris;
                while(curr != NULL){
                    fprintf(fout, "%-12s %-20s %-8d %-10d %-10d %-8d %-8d\n",
                           curr->id, curr->nama, curr->stokTotal, curr->tersedia,
                           curr->dipinjam, curr->rusak, curr->habis);
                    curr = curr->next;
                }
                fprintf(fout, "------------------------------------------------------------------------------------\n\n");
            }
        }
        // 5. TAMBAH STOK
        else if (strcmp(command, "ADD_STOCK") == 0) {
            char id[15];
            int jumlah;
            fscanf(fin, "%s %d", id, &jumlah);
            
            tambahStok(inventaris, id, jumlah, &status);
            fprintf(fout, "[TEST: ADD_STOCK] Tambah stok %d unit ke ID %s\n", jumlah, id);
            fprintf(fout, "   -> HASIL: %s\n\n", getStatusText(status));
        }
        // 6. KURANGI STOK
        else if (strcmp(command, "REDUCE_STOCK") == 0) {
            char id[15];
            int jumlah;
            fscanf(fin, "%s %d", id, &jumlah);
            
            kurangiStok(inventaris, id, jumlah, &status);
            fprintf(fout, "[TEST: REDUCE_STOCK] Kurangi stok %d unit dari ID %s\n", jumlah, id);
            fprintf(fout, "   -> HASIL: %s\n\n", getStatusText(status));
        }
        // 7. PINJAM ITEM
        else if (strcmp(command, "PINJAM") == 0) {
            char id[15];
            int jumlah;
            fscanf(fin, "%s %d", id, &jumlah);
            
            pinjamItem(inventaris, id, jumlah, &status);
            fprintf(fout, "[TEST: PINJAM] Pinjam %d unit dari ID %s\n", jumlah, id);
            fprintf(fout, "   -> HASIL: %s\n\n", getStatusText(status));
        }
        // 8. KEMBALIKAN ITEM
        else if (strcmp(command, "KEMBALI") == 0) {
            char id[15];
            int jumlah;
            fscanf(fin, "%s %d", id, &jumlah);
            
            kembalikanItem(inventaris, id, jumlah, &status);
            fprintf(fout, "[TEST: KEMBALI] Kembalikan %d unit ke ID %s\n", jumlah, id);
            fprintf(fout, "   -> HASIL: %s\n\n", getStatusText(status));
        }
        // 9. TANDAI RUSAK
        else if (strcmp(command, "RUSAK") == 0) {
            char id[15];
            int jumlah;
            fscanf(fin, "%s %d", id, &jumlah);
            
            tandaiRusak(inventaris, id, jumlah, &status);
            fprintf(fout, "[TEST: RUSAK] Tandai rusak %d unit pada ID %s\n", jumlah, id);
            fprintf(fout, "   -> HASIL: %s\n\n", getStatusText(status));
        }
        // 10. TANDAI HABIS
        else if (strcmp(command, "HABIS") == 0) {
            char id[15];
            int jumlah;
            fscanf(fin, "%s %d", id, &jumlah);
            
            tandaiHabis(inventaris, id, jumlah, &status);
            fprintf(fout, "[TEST: HABIS] Tandai habis %d unit pada ID %s\n", jumlah, id);
            fprintf(fout, "   -> HASIL: %s\n\n", getStatusText(status));
        }
        // 11. RINGKASAN INVENTARIS
        else if (strcmp(command, "SUMMARY") == 0) {
            fprintf(fout, "[TEST: SUMMARY] Menghitung rekapitulasi inventaris\n");
            int tItem = 0, tStok = 0, tSedia = 0, tPinjam = 0, tRusak = 0, tHabis = 0;
            List curr = inventaris;
            while(curr != NULL){
                tItem++; tStok += curr->stokTotal; tSedia += curr->tersedia;
                tPinjam += curr->dipinjam; tRusak += curr->rusak; tHabis += curr->habis;
                curr = curr->next;
            }
            fprintf(fout, "   -> Total Item Unik : %d\n", tItem);
            fprintf(fout, "   -> Total Unit Fisik: %d\n", tStok);
            fprintf(fout, "   -> Total Tersedia  : %d\n", tSedia);
            fprintf(fout, "   -> Sedang Dipinjam : %d\n", tPinjam);
            fprintf(fout, "   -> Kondisi Rusak   : %d\n", tRusak);
            fprintf(fout, "   -> Kondisi Habis   : %d\n\n", tHabis);
        }
    }

    fprintf(fout, "=================================================\n");
    fprintf(fout, "        TESTBENCH SELESAI DIEKSEKUSI\n");
    fprintf(fout, "=================================================\n");
    
    fclose(fin);
    fclose(fout);
    
    printf("Testing Comprehensive selesai! Cek file test/output/result.txt\n");

    return 0;
}