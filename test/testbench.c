#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/data.h"
#include "../include/inventory.h"
#include "../include/memory.h"
#include "../include/validation.h"
#include "../include/lookup.h"

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
        case 8: return "Nama barang sudah terdaftar";
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

    initLookup();

    fprintf(fout, "=================================================\n");
    fprintf(fout, "        AUTOMATED TESTBENCH - INVENTARIS\n");
    fprintf(fout, "=================================================\n\n");

    while (fscanf(fin, "%s", command) != EOF) {
        // 1. TAMBAH ITEM
        if (strcmp(command, "ADD") == 0) {
            Item data;
            char kat[32], lok[32], pem[32], pic[32];
            int tempTersedia;

            fscanf(fin, "%19s %31s %31s %31s %31s %d",
                   data.nama, kat, lok, pem, pic, &tempTersedia);

            data.kategoriIdx = registerLookup(TABLE_KATEGORI, kat);
            data.lokasiIdx = registerLookup(TABLE_LOKASI, lok);
            data.pemilikIdx = registerLookup(TABLE_PEMILIK, pem);
            data.picIdx = registerLookup(TABLE_PIC, pic);
            data.tersedia = (uint8_t)tempTersedia;
            data.dipinjam = 0;
            data.rusak = 0;
            data.habis = 0;
            
            tambahItem(&inventaris, &data, &status);
            fprintf(fout, "[TEST: ADD] Menambahkan ID: %s | Nama: %s | Stok: %d\n", data.id, data.nama, data.tersedia);
            fprintf(fout, "   -> HASIL: %s\n\n", getStatusText(status));
        }
        // 2. HAPUS ITEM
        else if (strcmp(command, "DELETE") == 0) {
            char id[8];
            fscanf(fin, "%7s", id);
            
            deleteItem(&inventaris, id, &status);
            fprintf(fout, "[TEST: DELETE] Menghapus ID %s dari sistem\n", id);
            fprintf(fout, "   -> HASIL: %s\n\n", getStatusText(status));
        }
        // 3. CARI ITEM
        else if (strcmp(command, "FIND") == 0) {
            char id[8];
            fscanf(fin, "%7s", id);
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
                fprintf(fout, "┌──────────┬──────────────────────┬──────────┬──────────┬──────────┬──────────┬──────────┐\n");
                fprintf(fout, "│ %-8s │ %-20s │ %-8s │ %-8s │ %-8s │ %-8s │ %-8s │\n",
                       "ID", "Nama Barang", "Total", "Tersedia", "Dipinjam", "Rusak", "Habis");
                fprintf(fout, "├──────────┼──────────────────────┼──────────┼──────────┼──────────┼──────────┼──────────┤\n");
                List curr = inventaris;
                while(curr != NULL){
                    fprintf(fout, "│ %-8s │ %-20s │ %8d │ %8d │ %8d │ %8d │ %8d │\n",
                           curr->id, curr->nama, getStokTotal(curr), curr->tersedia,
                           curr->dipinjam, curr->rusak, curr->habis);
                    curr = curr->next;
                }
                fprintf(fout, "└──────────┴──────────────────────┴──────────┴──────────┴──────────┴──────────┴──────────┘\n\n");
            }
        }
        // 5. TAMBAH STOK
        else if (strcmp(command, "ADD_STOCK") == 0) {
            char id[8];
            int jumlah;
            fscanf(fin, "%7s %d", id, &jumlah);
            
            tambahStok(inventaris, id, jumlah, &status);
            fprintf(fout, "[TEST: ADD_STOCK] Tambah stok %d unit ke ID %s\n", jumlah, id);
            fprintf(fout, "   -> HASIL: %s\n\n", getStatusText(status));
        }
        // 6. KURANGI STOK
        else if (strcmp(command, "REDUCE_STOCK") == 0) {
            char id[8];
            int jumlah;
            fscanf(fin, "%7s %d", id, &jumlah);
            
            kurangiStok(inventaris, id, jumlah, &status);
            fprintf(fout, "[TEST: REDUCE_STOCK] Kurangi stok %d unit dari ID %s\n", jumlah, id);
            fprintf(fout, "   -> HASIL: %s\n\n", getStatusText(status));
        }
        // 7. PINJAM ITEM
        else if (strcmp(command, "PINJAM") == 0) {
            char id[8];
            int jumlah;
            fscanf(fin, "%7s %d", id, &jumlah);
            
            pinjamItem(inventaris, id, jumlah, &status);
            fprintf(fout, "[TEST: PINJAM] Pinjam %d unit dari ID %s\n", jumlah, id);
            fprintf(fout, "   -> HASIL: %s\n\n", getStatusText(status));
        }
        // 8. KEMBALIKAN ITEM
        else if (strcmp(command, "KEMBALI") == 0) {
            char id[8];
            int jumlah;
            fscanf(fin, "%7s %d", id, &jumlah);
            
            kembalikanItem(inventaris, id, jumlah, &status);
            fprintf(fout, "[TEST: KEMBALI] Kembalikan %d unit ke ID %s\n", jumlah, id);
            fprintf(fout, "   -> HASIL: %s\n\n", getStatusText(status));
        }
        // 9. TANDAI RUSAK
        else if (strcmp(command, "RUSAK") == 0) {
            char id[8];    
            int jumlah;
            fscanf(fin, "%7s %d", id, &jumlah);
            
            tandaiRusak(inventaris, id, jumlah, &status);
            fprintf(fout, "[TEST: RUSAK] Tandai rusak %d unit pada ID %s\n", jumlah, id);
            fprintf(fout, "   -> HASIL: %s\n\n", getStatusText(status));
        }
        // 10. TANDAI HABIS
        else if (strcmp(command, "HABIS") == 0) {
            char id[8];
            int jumlah;
            fscanf(fin, "%7s %d", id, &jumlah);
            
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
                tItem++; tStok += getStokTotal(curr); tSedia += curr->tersedia;
                tPinjam += curr->dipinjam; tRusak += curr->rusak; tHabis += curr->habis;
                curr = curr->next;
            }
            fprintf(fout, "┌────────────────────────────────────────┐\n");
            fprintf(fout, "│          RINGKASAN INVENTARIS          │\n");
            fprintf(fout, "├────────────────────────────────────────┤\n");
            fprintf(fout, "│ Total Jenis Barang : %-17d │\n", tItem);
            fprintf(fout, "│ Total Unit Fisik   : %-17d │\n", tStok);
            fprintf(fout, "├────────────────────────────────────────┤\n");
            fprintf(fout, "│ Tersedia  : %4d unit (%5.1f%%)         │\n", tSedia, (tStok > 0) ? (tSedia * 100.0 / tStok) : 0.0);
            fprintf(fout, "│ Dipinjam  : %4d unit (%5.1f%%)         │\n", tPinjam, (tStok > 0) ? (tPinjam * 100.0 / tStok) : 0.0);
            fprintf(fout, "│ Rusak     : %4d unit (%5.1f%%)         │\n", tRusak, (tStok > 0) ? (tRusak * 100.0 / tStok) : 0.0);
            fprintf(fout, "│ Habis     : %4d unit (%5.1f%%)         │\n", tHabis, (tStok > 0) ? (tHabis * 100.0 / tStok) : 0.0);
            fprintf(fout, "└────────────────────────────────────────┘\n\n");
        }
    }

    fprintf(fout, "=================================================\n");
    fprintf(fout, "        TESTBENCH SELESAI DIEKSEKUSI\n");
    fprintf(fout, "=================================================\n");
    
    // Free LL
    List curr_node = inventaris;
    while(curr_node != NULL) {
        List temp = curr_node->next;
        free(curr_node);
        curr_node = temp;
    }

    fclose(fin);
    fclose(fout);
    
    freeLookup();
    
    printf("Testing Comprehensive selesai! Cek file test/output/result.txt\n");

    return 0;
}