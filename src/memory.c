#include <stdlib.h>
#include <string.h>
#include "../include/memory.h"

void insertLast(List *L, item data, int *status){
    List baru, curr;
    baru = (List) malloc(sizeof(item));
    if(baru == NULL){
        *status = 5;
        return;
    }
    strcpy(baru->id, data.id);
    strcpy(baru->nama, data.nama);
    strcpy(baru->kategori, data.kategori);
    strcpy(baru->lokasi, data.lokasi);
    strcpy(baru->pemilik, data.pemilik);
    strcpy(baru->pic, data.pic);
    baru->stokTotal = data.stokTotal;
    baru->tersedia = data.tersedia;
    baru->dipinjam = data.dipinjam;
    baru->rusak = data.rusak;
    baru->habis = data.habis;
    baru->next = NULL;
    if(*L == NULL){
        *L = baru;
    }
    else{
        curr = *L;
        while(curr->next != NULL){
            curr = curr->next;
        }
        curr->next = baru;
    }
    *status = 0;
}

void deleteItem(List *L, char id[], int *status){
    List curr, prev;
    curr = *L;
    prev = NULL;
    while(curr != NULL){
        if(strcmp(curr->id, id) == 0){
            if(curr->dipinjam > 0){
                *status = 7;
                return;
            }
            if(prev == NULL){
                *L = curr->next;
            }
            else{
                prev->next = curr->next;
            }
            free(curr);
            *status = 0;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    *status = 1;
}

void isEmpty(List L, int *kosong){
    if(L == NULL){
        *kosong = 1;
    }
    else{
        *kosong = 0;
    }
}