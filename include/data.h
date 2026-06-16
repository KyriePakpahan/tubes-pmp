#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include <stddef.h>

typedef struct item{
    char id[8];          
    char nama[20];      
    char kategori[15];   
    char lokasi[8];   
    char pemilik[10];    
    char pic[8];        
    int16_t tersedia;
    int16_t dipinjam;
    int16_t rusak;
    int16_t habis;
    struct item *next;
} item;

typedef item* List;

static inline int16_t getStokTotal(const item *it) {
    if (it == NULL) return 0;
    return it->tersedia + it->dipinjam + it->rusak + it->habis;
}

#endif
