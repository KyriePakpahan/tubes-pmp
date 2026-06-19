#include <stdlib.h>
#include <string.h>
#include "validation.h"
#include "memory.h"
#include "inventory.h"

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
#ifdef ARDUINO
    extern unsigned int __heap_start;
    extern void *__brkval;
    int v;
    int freeRAM = (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);

    int needed = (int)sizeof(Item) + 200;

    if(freeRAM < needed){
        *penuh = 1;
    }
    else{
        *penuh = 0;
    }
#else
    // Desktop always has plenty of memory
    *penuh = 0;
#endif
}

void normalisasiNama(char *nama) {
    if (nama == NULL || nama[0] == '\0') return;

    int len = strlen(nama);
    
    int start = 0;
    while (start < len && (nama[start] == ' ' || nama[start] == '\t' || nama[start] == '\r')) {
        start++;
    }

    int end = len - 1;
    while (end >= start && (nama[end] == ' ' || nama[end] == '\t' || nama[end] == '\r' || nama[end] == '\n')) {
        end--;
    }

    int writeIdx = 0;
    int isNewWord = 1; 

    for (int i = start; i <= end; i++) {
        char c = nama[i];

        if (c == ' ' || c == '\t' || c == '_') {
            if (writeIdx > 0 && nama[writeIdx - 1] != ' ') {
                nama[writeIdx++] = ' ';
                isNewWord = 1;
            }
        } else {
            if (isNewWord) {
                if (c >= 'a' && c <= 'z') {
                    nama[writeIdx++] = c - ('a' - 'A');
                } else {
                    nama[writeIdx++] = c;
                }
                isNewWord = 0;
            } else {
                if (c >= 'A' && c <= 'Z') {
                    nama[writeIdx++] = c + ('a' - 'A');
                } else {
                    nama[writeIdx++] = c;
                }
            }
        }
    }
    nama[writeIdx] = '\0';
}

void checkDuplicateName(List L, const char *nama, int *duplicate) {
    *duplicate = 0;
    List curr = L;
    while (curr != NULL) {
        if (strcmp(curr->nama, nama) == 0) {
            *duplicate = 1;
            return;
        }
        curr = curr->next;
    }
}