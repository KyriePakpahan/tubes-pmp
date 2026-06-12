#include <stdlib.h>
#include <string.h>
#include "../include/validation.h"

void checkDuplicate(List L, char id[], int *duplicate){
    List curr;
    *duplicate = 0;
    curr = L;
    while(curr != NULL){
        if(strcmp(curr->id, id) == 0){
            *duplicate = 1;
            return;
        }
        curr = curr->next;
    }
}

void checkPositive(int jumlah, int *valid){
    if(jumlah > 0){
        *valid = 1;
    }
    else{
        *valid = 0;
    }
}

void checkMemory(int *penuh){
    List test;
    test = (List) malloc(sizeof(item));
    if(test == NULL){
        *penuh = 1;
    }
    else{
        *penuh = 0;
        free(test);
    }
}