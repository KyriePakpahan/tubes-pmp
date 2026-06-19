#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./include/data.h"
#include "./include/memory.h"
#include "./include/validation.h"
#include "./include/inventory.h"
#include "./include/lookup.h"

// ANSI Color Escape Codes
#define ANSI_RESET   "\033[0m"
#define ANSI_BOLD    "\033[1m"
#define ANSI_RED     "\033[31m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN    "\033[36m"
#define ANSI_WHITE   "\033[37m"
#define ANSI_GRAY    "\033[90m"

void hapusEnter(char teks[]){
    int i = 0;
    while(teks[i] != '\0'){
        if(teks[i] == '\n'){
            teks[i] = '\0';
            return;
        }
        i++;
    }
}

void tampilStatus(int status){
    switch(status){
        case 0:
            printf("\n" ANSI_BOLD ANSI_GREEN "✔ SUCCESS: Operasi berhasil." ANSI_RESET "\n");
            break;
        case 1:
            printf("\n" ANSI_BOLD ANSI_RED "✘ ERROR: ID tidak ditemukan." ANSI_RESET "\n");
            break;
        case 2:
            printf("\n" ANSI_BOLD ANSI_RED "✘ ERROR: Jumlah tidak valid." ANSI_RESET "\n");
            break;
        case 3:
            printf("\n" ANSI_BOLD ANSI_RED "✘ ERROR: Stok tidak mencukupi." ANSI_RESET "\n");
            break;
        case 4:
            printf("\n" ANSI_BOLD ANSI_RED "✘ ERROR: ID sudah terdaftar." ANSI_RESET "\n");
            break;
        case 5:
            printf("\n" ANSI_BOLD ANSI_RED "✘ ERROR: Memori penuh." ANSI_RESET "\n");
            break;
        case 6:
            printf("\n" ANSI_BOLD ANSI_RED "✘ ERROR: Jumlah pengembalian melebihi jumlah yang dipinjam." ANSI_RESET "\n");
            break;
        case 7:
            printf("\n" ANSI_BOLD ANSI_RED "✘ ERROR: Item masih dipinjam sehingga tidak dapat dihapus." ANSI_RESET "\n");
            break;
        case 8:
            printf("\n" ANSI_BOLD ANSI_RED "✘ ERROR: Nama barang sudah terdaftar." ANSI_RESET "\n");
            break;
    }
}

int main(){
    List inventaris = NULL;
    List curr;
    Item data;
    int menu;
    int status;
    int jumlah;
    char id[8];
    char tempBuf[64];

    initLookup();

    do{
        printf("\n");
        printf(ANSI_GRAY "┌────────────────────────────────────────────────────────┐" ANSI_RESET "\n");
        printf(ANSI_GRAY "│" ANSI_RESET "                 " ANSI_BOLD ANSI_CYAN "SISTEM INVENTARIS LAB" ANSI_RESET "                  " ANSI_GRAY "│" ANSI_RESET "\n");
        printf(ANSI_GRAY "├────────────────────────────────────────────────────────┤" ANSI_RESET "\n");
        printf(ANSI_GRAY "│" ANSI_RESET "  [" ANSI_CYAN "1" ANSI_RESET "] Tambah Item            [" ANSI_CYAN "7" ANSI_RESET "] Pinjam Item           " ANSI_GRAY "│" ANSI_RESET "\n");
        printf(ANSI_GRAY "│" ANSI_RESET "  [" ANSI_CYAN "2" ANSI_RESET "] Hapus Item             [" ANSI_CYAN "8" ANSI_RESET "] Kembalikan Item       " ANSI_GRAY "│" ANSI_RESET "\n");
        printf(ANSI_GRAY "│" ANSI_RESET "  [" ANSI_CYAN "3" ANSI_RESET "] Cari Item              [" ANSI_CYAN "9" ANSI_RESET "] Tandai Rusak          " ANSI_GRAY "│" ANSI_RESET "\n");
        printf(ANSI_GRAY "│" ANSI_RESET "  [" ANSI_CYAN "4" ANSI_RESET "] Tampilkan Semua        [" ANSI_CYAN "10" ANSI_RESET "] Tandai Habis         " ANSI_GRAY "│" ANSI_RESET "\n");
        printf(ANSI_GRAY "│" ANSI_RESET "  [" ANSI_CYAN "5" ANSI_RESET "] Tambah Stok            [" ANSI_CYAN "11" ANSI_RESET "] Ringkasan            " ANSI_GRAY "│" ANSI_RESET "\n");
        printf(ANSI_GRAY "│" ANSI_RESET "  [" ANSI_CYAN "6" ANSI_RESET "] Kurangi Stok           [" ANSI_RED "0" ANSI_RESET "] Keluar                " ANSI_GRAY "│" ANSI_RESET "\n");
        printf(ANSI_GRAY "└────────────────────────────────────────────────────────┘" ANSI_RESET "\n");
        printf(ANSI_BOLD "Pilihan : " ANSI_RESET);

        if(scanf("%d", &menu) != 1) {
            // clear stdin buffer on invalid numerical input
            while(getchar() != '\n');
            printf("\n" ANSI_BOLD ANSI_RED "⚠ Masukkan angka menu yang valid." ANSI_RESET "\n");
            continue;
        }
        getchar();

        switch(menu){
            case 1:
                printf("\n" ANSI_BOLD ANSI_CYAN ">>> TAMBAH ITEM BARU <<<" ANSI_RESET "\n");
                printf("Nama Item : ");
                fgets(data.nama, sizeof(data.nama), stdin);
                hapusEnter(data.nama);
                
                printf("Kategori  : ");
                fgets(tempBuf, sizeof(tempBuf), stdin);
                hapusEnter(tempBuf);
                data.kategoriIdx = registerLookup(TABLE_KATEGORI, tempBuf);
                
                printf("Lokasi    : ");
                fgets(tempBuf, sizeof(tempBuf), stdin);
                hapusEnter(tempBuf);
                data.lokasiIdx = registerLookup(TABLE_LOKASI, tempBuf);
                
                printf("Pemilik   : ");
                fgets(tempBuf, sizeof(tempBuf), stdin);
                hapusEnter(tempBuf);
                data.pemilikIdx = registerLookup(TABLE_PEMILIK, tempBuf);
                
                printf("PIC       : ");
                fgets(tempBuf, sizeof(tempBuf), stdin);
                hapusEnter(tempBuf);
                data.picIdx = registerLookup(TABLE_PIC, tempBuf);
                
                printf("Stok Awal : ");
                int tempTersedia;
                if(scanf("%d", &tempTersedia) != 1) {
                    while(getchar() != '\n');
                    printf("\n" ANSI_BOLD ANSI_RED "✘ ERROR: Stok awal harus berupa angka." ANSI_RESET "\n");
                    break;
                }
                getchar();
                if(tempTersedia <= 0 || tempTersedia > 255){
                    printf("\n" ANSI_BOLD ANSI_RED "✘ ERROR: Stok awal harus antara 1-255." ANSI_RESET "\n");
                    break;
                }
                data.tersedia = (uint8_t)tempTersedia;
                data.dipinjam = 0;
                data.rusak = 0;
                data.habis = 0;
                
                tambahItem(&inventaris, &data, &status);
                if (status == 0) {
                    printf("\n" ANSI_BOLD ANSI_GREEN "✔ SUCCESS: Item ditambahkan dengan ID: %s" ANSI_RESET "\n", data.id);
                } else {
                    tampilStatus(status);
                }
                break;
            
            case 2:
                printf("\n" ANSI_BOLD ANSI_RED ">>> HAPUS ITEM <<<" ANSI_RESET "\n");
                printf("Masukkan ID Item : ");
                fgets(id, sizeof(id), stdin);
                hapusEnter(id);
                deleteItem(&inventaris, id, &status);
                tampilStatus(status);
                break;

            case 3:
                printf("\n" ANSI_BOLD ANSI_CYAN ">>> CARI ITEM <<<" ANSI_RESET "\n");
                printf("Masukkan ID Item : ");
                fgets(id, sizeof(id), stdin);
                hapusEnter(id);
                curr = inventaris;
                while(curr != NULL){
                    if(strcmp(curr->id, id) == 0){
                        char kat[32], lok[32], pem[32], pic[32];
                        getLookupString(TABLE_KATEGORI, curr->kategoriIdx, kat, sizeof(kat));
                        getLookupString(TABLE_LOKASI, curr->lokasiIdx, lok, sizeof(lok));
                        getLookupString(TABLE_PEMILIK, curr->pemilikIdx, pem, sizeof(pem));
                        getLookupString(TABLE_PIC, curr->picIdx, pic, sizeof(pic));

                        printf("\n" ANSI_GRAY "┌────────────────────────────────────────┐" ANSI_RESET "\n");
                        printf(ANSI_GRAY "│" ANSI_RESET " " ANSI_BOLD ANSI_CYAN "             DETAIL BARANG            " ANSI_RESET " " ANSI_GRAY "│" ANSI_RESET "\n");
                        printf(ANSI_GRAY "├────────────────────────────────────────┤" ANSI_RESET "\n");
                        printf(ANSI_GRAY "│" ANSI_RESET " ID           : %-22s " ANSI_GRAY "│" ANSI_RESET "\n", curr->id);
                        printf(ANSI_GRAY "│" ANSI_RESET " Nama         : %-22s " ANSI_GRAY "│" ANSI_RESET "\n", curr->nama);
                        printf(ANSI_GRAY "│" ANSI_RESET " Kategori     : %-22s " ANSI_GRAY "│" ANSI_RESET "\n", kat);
                        printf(ANSI_GRAY "│" ANSI_RESET " Lokasi       : %-22s " ANSI_GRAY "│" ANSI_RESET "\n", lok);
                        printf(ANSI_GRAY "│" ANSI_RESET " Pemilik      : %-22s " ANSI_GRAY "│" ANSI_RESET "\n", pem);
                        printf(ANSI_GRAY "│" ANSI_RESET " PIC          : %-22s " ANSI_GRAY "│" ANSI_RESET "\n", pic);
                        printf(ANSI_GRAY "├────────────────────────────────────────┤" ANSI_RESET "\n");
                        printf(ANSI_GRAY "│" ANSI_RESET " Stok Total   : %-22d " ANSI_GRAY "│" ANSI_RESET "\n", getStokTotal(curr));
                        printf(ANSI_GRAY "│" ANSI_RESET " Tersedia     : " ANSI_GREEN "%-22d" ANSI_RESET " " ANSI_GRAY "│" ANSI_RESET "\n", curr->tersedia);
                        printf(ANSI_GRAY "│" ANSI_RESET " Dipinjam     : " ANSI_YELLOW "%-22d" ANSI_RESET " " ANSI_GRAY "│" ANSI_RESET "\n", curr->dipinjam);
                        printf(ANSI_GRAY "│" ANSI_RESET " Rusak        : " ANSI_RED "%-22d" ANSI_RESET " " ANSI_GRAY "│" ANSI_RESET "\n", curr->rusak);
                        printf(ANSI_GRAY "│" ANSI_RESET " Habis        : " ANSI_RED "%-22d" ANSI_RESET " " ANSI_GRAY "│" ANSI_RESET "\n", curr->habis);
                        printf(ANSI_GRAY "└────────────────────────────────────────┘" ANSI_RESET "\n");
                        break;
                    }
                    curr = curr->next;
                }
                if(curr == NULL){
                    printf("\n" ANSI_BOLD ANSI_RED "✘ ERROR: ID tidak ditemukan." ANSI_RESET "\n");
                }
                break;
 
            case 4:
                if(inventaris == NULL){
                    printf("\n" ANSI_BOLD ANSI_YELLOW "⚠ Data inventaris kosong." ANSI_RESET "\n");
                    break;
                }
                printf("\n" ANSI_GRAY "┌──────────┬──────────────────────┬──────────┬──────────┬──────────┬──────────┬──────────┐" ANSI_RESET "\n");
                printf(ANSI_GRAY "│" ANSI_RESET " " ANSI_BOLD ANSI_CYAN "%-8s" ANSI_RESET " " ANSI_GRAY "│" ANSI_RESET " " ANSI_BOLD ANSI_CYAN "%-20s" ANSI_RESET " " ANSI_GRAY "│" ANSI_RESET " " ANSI_BOLD ANSI_CYAN "%-8s" ANSI_RESET " " ANSI_GRAY "│" ANSI_RESET " " ANSI_BOLD ANSI_CYAN "%-8s" ANSI_RESET " " ANSI_GRAY "│" ANSI_RESET " " ANSI_BOLD ANSI_CYAN "%-8s" ANSI_RESET " " ANSI_GRAY "│" ANSI_RESET " " ANSI_BOLD ANSI_CYAN "%-8s" ANSI_RESET " " ANSI_GRAY "│" ANSI_RESET " " ANSI_BOLD ANSI_CYAN "%-8s" ANSI_RESET " " ANSI_GRAY "│" ANSI_RESET "\n",
                       "ID", "Nama Barang", "Total", "Tersedia", "Dipinjam", "Rusak", "Habis");
                printf(ANSI_GRAY "├──────────┼──────────────────────┼──────────┼──────────┼──────────┼──────────┼──────────┤" ANSI_RESET "\n");
                curr = inventaris;
                while(curr != NULL){
                    printf(ANSI_GRAY "│" ANSI_RESET " %-8s " ANSI_GRAY "│" ANSI_RESET " %-20s " ANSI_GRAY "│" ANSI_RESET " %8d " ANSI_GRAY "│" ANSI_RESET " " ANSI_GREEN "%8d" ANSI_RESET " " ANSI_GRAY "│" ANSI_RESET " " ANSI_YELLOW "%8d" ANSI_RESET " " ANSI_GRAY "│" ANSI_RESET " " ANSI_RED "%8d" ANSI_RESET " " ANSI_GRAY "│" ANSI_RESET " " ANSI_RED "%8d" ANSI_RESET " " ANSI_GRAY "│" ANSI_RESET "\n",
                           curr->id,
                           curr->nama,
                           getStokTotal(curr),
                           curr->tersedia,
                           curr->dipinjam,
                           curr->rusak,
                           curr->habis);

                    curr = curr->next;
                }
                printf(ANSI_GRAY "└──────────┴──────────────────────┴──────────┴──────────┴──────────┴──────────┴──────────┘" ANSI_RESET "\n");
                break;

            case 5:
                printf("\n" ANSI_BOLD ANSI_CYAN ">>> TAMBAH STOK <<<" ANSI_RESET "\n");
                printf("Masukkan ID Item : ");
                fgets(id, sizeof(id), stdin);
                hapusEnter(id);
                printf("Jumlah stok yang ditambahkan : ");
                scanf("%d", &jumlah);
                getchar();
                tambahStok(inventaris, id, jumlah, &status);
                tampilStatus(status);
                break;

            case 6:
                printf("\n" ANSI_BOLD ANSI_RED ">>> KURANGI STOK <<<" ANSI_RESET "\n");
                printf("Masukkan ID Item : ");
                fgets(id, sizeof(id), stdin);
                hapusEnter(id);
                printf("Jumlah stok yang dikurangi : ");
                scanf("%d", &jumlah);
                getchar();
                kurangiStok(inventaris, id, jumlah, &status);
                tampilStatus(status);
                break;

            case 7:
                printf("\n" ANSI_BOLD ANSI_YELLOW ">>> PINJAM ITEM <<<" ANSI_RESET "\n");
                printf("Masukkan ID Item : ");
                fgets(id, sizeof(id), stdin);
                hapusEnter(id);
                printf("Jumlah yang dipinjam : ");
                scanf("%d", &jumlah);
                getchar();
                pinjamItem(inventaris, id, jumlah, &status);
                tampilStatus(status);
                break;

            case 8:
                printf("\n" ANSI_BOLD ANSI_GREEN ">>> KEMBALIKAN ITEM <<<" ANSI_RESET "\n");
                printf("Masukkan ID Item : ");
                fgets(id, sizeof(id), stdin);
                hapusEnter(id);
                printf("Jumlah yang dikembalikan : ");
                scanf("%d", &jumlah);
                getchar();
                kembalikanItem(inventaris, id, jumlah, &status);
                tampilStatus(status);
                break;

            case 9:
                printf("\n" ANSI_BOLD ANSI_RED ">>> TANDAI RUSAK <<<" ANSI_RESET "\n");
                printf("Masukkan ID Item : ");
                fgets(id, sizeof(id), stdin);
                hapusEnter(id);
                printf("Jumlah item rusak : ");
                scanf("%d", &jumlah);
                getchar();
                tandaiRusak(inventaris, id, jumlah, &status);
                tampilStatus(status);
                break;

            case 10:
                printf("\n" ANSI_BOLD ANSI_RED ">>> TANDAI HABIS <<<" ANSI_RESET "\n");
                printf("Masukkan ID Item : ");
                fgets(id, sizeof(id), stdin);
                hapusEnter(id);
                printf("Jumlah item habis : ");
                scanf("%d", &jumlah);
                getchar();
                tandaiHabis(inventaris, id, jumlah, &status);
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

                printf("\n" ANSI_GRAY "┌──────────────────────────────────────────────────────────────────────┐" ANSI_RESET "\n");
                printf(ANSI_GRAY "│" ANSI_RESET " " ANSI_BOLD ANSI_CYAN "                         RINGKASAN INVENTARIS                         " ANSI_RESET " " ANSI_GRAY "│" ANSI_RESET "\n");
                printf(ANSI_GRAY "├──────────────────────────────────────────────────────────────────────┤" ANSI_RESET "\n");
                printf(ANSI_GRAY "│" ANSI_RESET "  Total Jenis Barang : " ANSI_BOLD "%-44d" ANSI_RESET "   " ANSI_GRAY "│" ANSI_RESET "\n", totalItem);
                printf(ANSI_GRAY "│" ANSI_RESET "  Total Unit Fisik   : " ANSI_BOLD "%-44d" ANSI_RESET "   " ANSI_GRAY "│" ANSI_RESET "\n", totalStok);
                printf(ANSI_GRAY "├──────────────────────────────────────────────────────────────────────┤" ANSI_RESET "\n");

                // Tersedia
                {
                    int fill = (totalStok > 0) ? (totalTersedia * 30 / totalStok) : 0;
                    double pct = (totalStok > 0) ? (totalTersedia * 100.0 / totalStok) : 0.0;
                    printf(ANSI_GRAY "│" ANSI_RESET "  %-10s : " ANSI_GREEN, "Tersedia");
                    for(int i=0; i<30; i++) {
                        if(i < fill) printf("■");
                        else printf(" ");
                    }
                    printf(ANSI_RESET "  %4d unit (%5.1f%%)    " ANSI_GRAY "│" ANSI_RESET "\n", totalTersedia, pct);
                }
                // Dipinjam
                {
                    int fill = (totalStok > 0) ? (totalDipinjam * 30 / totalStok) : 0;
                    double pct = (totalStok > 0) ? (totalDipinjam * 100.0 / totalStok) : 0.0;
                    printf(ANSI_GRAY "│" ANSI_RESET "  %-10s : " ANSI_YELLOW, "Dipinjam");
                    for(int i=0; i<30; i++) {
                        if(i < fill) printf("■");
                        else printf(" ");
                    }
                    printf(ANSI_RESET "  %4d unit (%5.1f%%)    " ANSI_GRAY "│" ANSI_RESET "\n", totalDipinjam, pct);
                }
                // Rusak
                {
                    int fill = (totalStok > 0) ? (totalRusak * 30 / totalStok) : 0;
                    double pct = (totalStok > 0) ? (totalRusak * 100.0 / totalStok) : 0.0;
                    printf(ANSI_GRAY "│" ANSI_RESET "  %-10s : " ANSI_RED, "Rusak");
                    for(int i=0; i<30; i++) {
                        if(i < fill) printf("■");
                        else printf(" ");
                    }
                    printf(ANSI_RESET "  %4d unit (%5.1f%%)    " ANSI_GRAY "│" ANSI_RESET "\n", totalRusak, pct);
                }
                // Habis
                {
                    int fill = (totalStok > 0) ? (totalHabis * 30 / totalStok) : 0;
                    double pct = (totalStok > 0) ? (totalHabis * 100.0 / totalStok) : 0.0;
                    printf(ANSI_GRAY "│" ANSI_RESET "  %-10s : " ANSI_RED, "Habis");
                    for(int i=0; i<30; i++) {
                        if(i < fill) printf("■");
                        else printf(" ");
                    }
                    printf(ANSI_RESET "  %4d unit (%5.1f%%)    " ANSI_GRAY "│" ANSI_RESET "\n", totalHabis, pct);
                }
                printf(ANSI_GRAY "└──────────────────────────────────────────────────────────────────────┘" ANSI_RESET "\n");
                break;
            }

            case 0:
                printf("\n" ANSI_BOLD ANSI_GREEN "Program selesai. Sampai jumpa!" ANSI_RESET "\n");
                break;
            default:
                printf("\n" ANSI_BOLD ANSI_RED "⚠ Menu tidak valid." ANSI_RESET "\n");
        }

    }while(menu != 0);

    // Free LL
    List curr_node = inventaris;
    while(curr_node != NULL) {
        List temp = curr_node->next;
        free(curr_node);
        curr_node = temp;
    }
    
    freeLookup();
    return 0;
}