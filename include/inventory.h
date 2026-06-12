#ifndef INVENTORY_H
#define INVENTORY_H
#include "data.h"

void tambahItem(List *L, item data, int *status);
void tambahStok(List L, char id[], int jumlah, int *status);
void kurangiStok(List L, char id[], int jumlah, int *status);
void pinjamItem(List L, char id[], int jumlah, int *status);
void kembalikanItem(List L, char id[], int jumlah, int *status);
void tandaiRusak(List L, char id[], int jumlah, int *status);
void tandaiHabis(List L, char id[], int jumlah, int *status);

#endif