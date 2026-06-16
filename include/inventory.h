#ifndef INVENTORY_H
#define INVENTORY_H

#include "data.h"

#ifdef __cplusplus
extern "C" {
#endif

void tambahItem(List *L, item data, int *status);
void deleteItem(List *L, char id[], int *status);
void tambahStok(List L, char id[], int jumlah, int *status);
void kurangiStok(List L, char id[], int jumlah, int *status);
void pinjamItem(List L, char id[], int jumlah, int *status);
void kembalikanItem(List L, char id[], int jumlah, int *status);
void tandaiRusak(List L, char id[], int jumlah, int *status);
void tandaiHabis(List L, char id[], int jumlah, int *status);

#ifdef __cplusplus
}
#endif

#endif