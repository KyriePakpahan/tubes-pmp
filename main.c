#include <stdio.h>
#include <string.h>
#include "./include/data.h"
#include "./include/memory.h"
#include "./include/validation.h"
#include "./include/inventory.h"

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
            printf("\nOperasi berhasil.\n");
            break;
        case 1:
            printf("\nID tidak ditemukan.\n");
            break;
        case 2:
            printf("\nJumlah tidak valid.\n");
            break;
        case 3:
            printf("\nStok tidak mencukupi.\n");
            break;
        case 4:
            printf("\nID sudah terdaftar.\n");
            break;
        case 5:
            printf("\nMemori penuh.\n");
            break;
        case 6:
            printf("\nJumlah pengembalian melebihi jumlah yang dipinjam.\n");
            break;
        case 7:
            printf("\nItem masih dipinjam sehingga tidak dapat dihapus.\n");
            break;
    }
}

int main(){
    List inventaris = NULL;
    List curr;
    item data;
    int menu;
    int status;
    int jumlah;
    char id[15];

    do{
        printf("\n");
        printf("=====================================\n");
        printf("      SISTEM INVENTARIS LAB\n");
        printf("=====================================\n");
        printf("1. Tambah Item\n");
        printf("2. Hapus Item\n");
        printf("3. Cari Item\n");
        printf("4. Tampilkan Semua Item\n");
        printf("5. Tambah Stok\n");
        printf("6. Kurangi Stok\n");
        printf("7. Pinjam Item\n");
        printf("8. Kembalikan Item\n");
        printf("9. Tandai Rusak\n");
        printf("10. Tandai Habis\n");
        printf("11. Ringkasan Inventaris\n");
        printf("0. Keluar\n");
        printf("=====================================\n");
        printf("Pilihan : ");

        scanf("%d", &menu);
        getchar();

        switch(menu){
            case 1:
                printf("\nID Item : ");
                fgets(data.id, 15, stdin);
                hapusEnter(data.id);
                printf("Nama Item : ");
                fgets(data.nama, 50, stdin);
                hapusEnter(data.nama);
                printf("Kategori : ");
                fgets(data.kategori, 30, stdin);
                hapusEnter(data.kategori);
                printf("Lokasi : ");
                fgets(data.lokasi, 30, stdin);
                hapusEnter(data.lokasi);
                printf("Pemilik : ");
                fgets(data.pemilik, 50, stdin);
                hapusEnter(data.pemilik);
                printf("PIC : ");
                fgets(data.pic, 50, stdin);
                hapusEnter(data.pic);
                printf("Stok Awal : ");
                scanf("%d", &data.stokTotal);
                getchar();
                if(data.stokTotal <= 0){
                    printf("\nStok awal harus lebih dari 0.\n");
                    break;
                }
                data.tersedia = data.stokTotal;
                data.dipinjam = 0;
                data.rusak = 0;
                data.habis = 0;
                tambahItem(&inventaris, data, &status);
                tampilStatus(status);
                break;
            
            case 2:
                printf("\nMasukkan ID Item : ");
                fgets(id, 15, stdin);
                hapusEnter(id);
                deleteItem(&inventaris, id, &status);
                tampilStatus(status);
                break;

            case 3:
                printf("\nMasukkan ID Item : ");
                fgets(id, 15, stdin);
                hapusEnter(id);
                curr = inventaris;
                while(curr != NULL){
                    if(strcmp(curr->id, id) == 0){
                        printf("\n=====================================\n");
                        printf("ID           : %s\n", curr->id);
                        printf("Nama         : %s\n", curr->nama);
                        printf("Kategori     : %s\n", curr->kategori);
                        printf("Lokasi       : %s\n", curr->lokasi);
                        printf("Pemilik      : %s\n", curr->pemilik);
                        printf("PIC          : %s\n", curr->pic);
                        printf("Stok Total   : %d\n", curr->stokTotal);
                        printf("Tersedia     : %d\n", curr->tersedia);
                        printf("Dipinjam     : %d\n", curr->dipinjam);
                        printf("Rusak        : %d\n", curr->rusak);
                        printf("Habis        : %d\n", curr->habis);
                        printf("=====================================\n");

                        break;
                    }
                    curr = curr->next;
                }
                if(curr == NULL){
                    printf("\nID tidak ditemukan.\n");
                }
                break;

            case 4:
                if(inventaris == NULL){
                    printf("\nData inventaris kosong.\n");
                    break;
                }
                printf("\n====================================================================================================\n");
                printf("%-12s %-20s %-8s %-10s %-10s %-8s %-8s\n",
                       "ID",
                       "Nama",
                       "Total",
                       "Tersedia",
                       "Dipinjam",
                       "Rusak",
                       "Habis");
                printf("====================================================================================================\n");
                curr = inventaris;
                while(curr != NULL){
                    printf("%-12s %-20s %-8d %-10d %-10d %-8d %-8d\n",
                           curr->id,
                           curr->nama,
                           curr->stokTotal,
                           curr->tersedia,
                           curr->dipinjam,
                           curr->rusak,
                           curr->habis);

                    curr = curr->next;
                }
                printf("====================================================================================================\n");
                break;

            case 5:
                printf("\nMasukkan ID Item : ");
                fgets(id, 15, stdin);
                hapusEnter(id);
                printf("Jumlah stok yang ditambahkan : ");
                scanf("%d", &jumlah);
                getchar();
                tambahStok(inventaris, id, jumlah, &status);
                tampilStatus(status);
                break;

            case 6:
                printf("\nMasukkan ID Item : ");
                fgets(id, 15, stdin);
                hapusEnter(id);
                printf("Jumlah stok yang dikurangi : ");
                scanf("%d", &jumlah);
                getchar();
                kurangiStok(inventaris, id, jumlah, &status);
                tampilStatus(status);
                break;

            case 7:
                printf("\nMasukkan ID Item : ");
                fgets(id, 15, stdin);
                hapusEnter(id);
                printf("Jumlah yang dipinjam : ");
                scanf("%d", &jumlah);
                getchar();
                pinjamItem(inventaris, id, jumlah, &status);
                tampilStatus(status);
                break;

            case 8:
                printf("\nMasukkan ID Item : ");
                fgets(id, 15, stdin);
                hapusEnter(id);
                printf("Jumlah yang dikembalikan : ");
                scanf("%d", &jumlah);
                getchar();
                kembalikanItem(inventaris, id, jumlah, &status);
                tampilStatus(status);
                break;

            case 9:
                printf("\nMasukkan ID Item : ");
                fgets(id, 15, stdin);
                hapusEnter(id);
                printf("Jumlah item rusak : ");
                scanf("%d", &jumlah);
                getchar();
                tandaiRusak(inventaris, id, jumlah, &status);
                tampilStatus(status);
                break;

            case 10:
                printf("\nMasukkan ID Item : ");
                fgets(id, 15, stdin);
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
                    totalStok += curr->stokTotal;
                    totalTersedia += curr->tersedia;
                    totalDipinjam += curr->dipinjam;
                    totalRusak += curr->rusak;
                    totalHabis += curr->habis;
                    curr = curr->next;
                }
                printf("\n========== RINGKASAN INVENTARIS ==========\n");
                printf("Jumlah Item      : %d\n", totalItem);
                printf("Total Stok       : %d\n", totalStok);
                printf("Total Tersedia   : %d\n", totalTersedia);
                printf("Total Dipinjam   : %d\n", totalDipinjam);
                printf("Total Rusak      : %d\n", totalRusak);
                printf("Total Habis      : %d\n", totalHabis);
                printf("==========================================\n");
                break;
            }

            case 0:
                printf("\nProgram selesai.\n");
                break;
            default:
                printf("\nMenu tidak valid.\n");
        }

    }while(menu != 0);
    return 0;
}