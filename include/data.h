#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include <stddef.h>

typedef struct Item{
    char id[8];          
    char nama[20];      
    uint8_t kategoriIdx;
    uint8_t lokasiIdx;
    uint8_t pemilikIdx;
    uint8_t picIdx;
    uint8_t tersedia;
    uint8_t dipinjam;
    uint8_t rusak;
    uint8_t habis;
    struct Item *next;
} Item;

typedef Item* List;

static inline int16_t getStokTotal(const Item *it) {
    if (it == NULL) return 0;
    return (int16_t)it->tersedia + it->dipinjam + it->rusak + it->habis;
}

#endif
