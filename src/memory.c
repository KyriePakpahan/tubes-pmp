#include <stdlib.h>
#include <string.h>
#include "memory.h"

int compareIDs(const char *id1, const char *id2) {
    if (id1 == NULL || id2 == NULL) return 0;
    if (id1[0] != id2[0]) {
        return id1[0] - id2[0];
    }

    int num1 = atoi(id1 + 1);
    int num2 = atoi(id2 + 1);
    return num1 - num2;
}

void insertSorted(List *L, Item data, int *status){
    List baru, curr, prev;
    baru = (List) malloc(sizeof(Item));
    if(baru == NULL){
        *status = 5;
        return;
    }
    memcpy(baru, &data, sizeof(Item));
    baru->next = NULL;
    
    if(*L == NULL){
        *L = baru;
    }
    else{
        curr = *L;
        prev = NULL;
        while(curr != NULL && compareIDs(curr->id, baru->id) < 0){
            prev = curr;
            curr = curr->next;
        }
        
        if(prev == NULL){
            baru->next = *L;
            *L = baru;
        }
        else{
            baru->next = curr;
            prev->next = baru;
        }
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