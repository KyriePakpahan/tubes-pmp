#ifndef DATA_H
#define DATA_H

typedef struct item{
    char id[15];
    char nama[50];
    char kategori[30];
    char lokasi[30];
    char pemilik[50];
    char pic[50];
    int stokTotal;
    int tersedia;
    int dipinjam;
    int rusak;
    int habis;
    struct item *next;
} item;

typedef item* List;

#endif