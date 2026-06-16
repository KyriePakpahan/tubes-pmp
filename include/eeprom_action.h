#ifndef EEPROM_ACTION_H
#define EEPROM_ACTION_H

#include "data.h"
#include <stdint.h>

#define EEPROM_SIZE 1024
#define MAGIC_NUMBER 0xAC
#define MAX_ITEMS 100

typedef void (*LogFunc)(const char *msg);

typedef struct {
    uint8_t magic;
    uint16_t itemCount;
    uint16_t checksum;
} EEPROMHeader;

#ifdef __cplusplus
extern "C" {
#endif

void saveToEEPROM(List L, LogFunc log);
void loadFromEEPROM(List *L, LogFunc log);
void clearEEPROM(LogFunc log);
void displayEEPROMInfo(LogFunc log);

#ifdef __cplusplus
}
#endif

#endif