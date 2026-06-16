#include <string.h>
#include "inventory.h"
#include "memory.h"
#include "validation.h"
#include "data.h"

void tambahItem(List *L, item data, int *status){
    int duplicate;
    int penuh;
    checkDuplicate(*L, data.id, &duplicate);
    if(duplicate == 1){
        *status = 4;
        return;
    }
    checkMemory(&penuh);
    if(penuh == 1){
        *status = 5;
        return;
    }
    insertLast(L, data, status);
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