#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "inventory.h"
#include "memory.h"
#include "validation.h"
#include "data.h"
#include "lookup.h"

void generateNextID(List L, const char *nama, char *destId) {
    char prefix = 'X'; 
    if (nama[0] != '\0') {
        prefix = nama[0];
        if (prefix >= 'a' && prefix <= 'z') {
            prefix = prefix - ('a' - 'A');
        }
    }

    int maxNum = 0;
    List curr = L;
    while (curr != NULL) {
        if (curr->id[0] == prefix) {
            int num = atoi(curr->id + 1);
            if (num > maxNum) {
                maxNum = num;
            }
        }
        curr = curr->next;
    }
    
    sprintf(destId, "%c%d", prefix, maxNum + 1);
}

void tambahItem(List *L, Item *data, int *status){
    // 1normalize item name
    normalisasiNama(data->nama);

    // check duplicate name
    int duplicateName;
    checkDuplicateName(*L, data->nama, &duplicateName);
    if(duplicateName == 1){
        *status = 8; 
        return;
    }

    // generate ID
    generateNextID(*L, data->nama, data->id);

    // check duplicate ID again
    int duplicateId;
    checkDuplicate(*L, data->id, &duplicateId);
    if(duplicateId == 1){
        *status = 4;
        return;
    }

    // check memory space
    int penuh;
    checkMemory(&penuh);
    if(penuh == 1){
        *status = 5;
        return;
    }

    // insert data ascending by ID
    insertSorted(L, *data, status);
}

void tambahStok(List L, char id[], int jumlah, int *status){
    List curr;
    int valid;
    checkPositive(jumlah, &valid);
    if(valid == 0){
        *status = 2;
        return;
    }
    curr = L;
    while(curr != NULL){
        if(strcmp(curr->id, id) == 0){
            curr->tersedia += jumlah;
            *status = 0;
            return;
        }
        curr = curr->next;
    }
    *status = 1;
}

void kurangiStok(List L, char id[], int jumlah, int *status){
    List curr;
    int valid;
    checkPositive(jumlah, &valid);
    if(valid == 0){
        *status = 2;
        return;
    }
    curr = L;
    while(curr != NULL){
        if(strcmp(curr->id, id) == 0){
            if(jumlah > curr->tersedia){
                *status = 3;
                return;
            }
            curr->tersedia -= jumlah;
            *status = 0;
            return;
        }
        curr = curr->next;
    }
    *status = 1;
}

void pinjamItem(List L, char id[], int jumlah, int *status){
    List curr;
    int valid;
    checkPositive(jumlah, &valid);
    if(valid == 0){
        *status = 2;
        return;
    }
    curr = L;
    while(curr != NULL){
        if(strcmp(curr->id, id) == 0){
            if(jumlah > curr->tersedia){
                *status = 3;
                return;
            }
            curr->tersedia -= jumlah;
            curr->dipinjam += jumlah;
            *status = 0;
            return;
        }
        curr = curr->next;
    }
    *status = 1;
}

void kembalikanItem(List L, char id[], int jumlah, int *status){
    List curr;
    int valid;
    checkPositive(jumlah, &valid);
    if(valid == 0){
        *status = 2;
        return;
    }
    curr = L;
    while(curr != NULL){
        if(strcmp(curr->id, id) == 0){
            if(jumlah > curr->dipinjam){
                *status = 6;
                return;
            }
            curr->dipinjam -= jumlah;
            curr->tersedia += jumlah;
            *status = 0;
            return;
        }
        curr = curr->next;
    }
    *status = 1;
}

void tandaiRusak(List L, char id[], int jumlah, int *status){
    List curr;
    int valid;
    checkPositive(jumlah, &valid);
    if(valid == 0){
        *status = 2;
        return;
    }
    curr = L;
    while(curr != NULL){
        if(strcmp(curr->id, id) == 0){
            if(jumlah > curr->tersedia){
                *status = 3;
                return;
            }
            curr->tersedia -= jumlah;
            curr->rusak += jumlah;
            *status = 0;
            return;
        }
        curr = curr->next;
    }
    *status = 1;
}

void tandaiHabis(List L, char id[], int jumlah, int *status){
    List curr;
    int valid;
    checkPositive(jumlah, &valid);
    if(valid == 0){
        *status = 2;
        return;
    }
    curr = L;
    while(curr != NULL){
        if(strcmp(curr->id, id) == 0){
            if(jumlah > curr->tersedia){
                *status = 3;
                return;
            }
            curr->tersedia -= jumlah;
            curr->habis += jumlah;
            *status = 0;
            return;
        }
        curr = curr->next;
    }
    *status = 1;
}