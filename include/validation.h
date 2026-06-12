#ifndef VALIDATION_H
#define VALIDATION_H
#include "data.h"

void checkDuplicate(List L, char id[], int *duplicate);
void checkPositive(int jumlah, int *valid);
void checkMemory(int *penuh);

#endif