#ifndef MEMORY_H
#define MEMORY_H

#include "data.h"

#ifdef __cplusplus
extern "C" {
#endif

int compareIDs(const char *id1, const char *id2);
void insertSorted(List *L, Item data, int *status);
void deleteItem(List *L, char id[], int *status);
void isEmpty(List L, int *kosong);

#ifdef __cplusplus
}
#endif

#endif