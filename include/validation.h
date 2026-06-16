#ifndef VALIDATION_H
#define VALIDATION_H

#include "data.h"

#ifdef __cplusplus
extern "C" {
#endif

void checkDuplicate(List L, char id[], int *duplicate);
void checkPositive(int value, int *valid);
void checkMemory(int *penuh);

#ifdef __cplusplus
}
#endif

#endif