#ifndef MEMORY_H
#define MEMORY_H
#include "data.h"

void insertLast(List *L, item data, int *status);
void deleteItem(List *L, char id[], int *status);
void isEmpty(List L, int *kosong);

#endif