#ifndef LOOKUP_H
#define LOOKUP_H

#include <stdint.h>
#include <stddef.h>

// arduino compatibility stubs
#ifndef ARDUINO
#define PROGMEM
#define PSTR(s) (s)
#define pgm_read_byte(addr) (*(const uint8_t *)(addr))
#define pgm_read_word(addr) (*(const uint16_t *)(addr))
#define pgm_read_ptr(addr) (*(const void **)(addr))
#define strcpy_P strcpy
#define strlen_P strlen
#endif

// lookup-table IDs
#define TABLE_KATEGORI 0
#define TABLE_LOKASI   1
#define TABLE_PEMILIK  2
#define TABLE_PIC      3

// max items in dynamic SRAM pool per table
#define MAX_CUSTOM 8

// invalid index representation
#define LOOKUP_INVALID 255

#ifdef __cplusplus
extern "C" {
#endif

void initLookup(void);
void freeLookup(void);

uint8_t registerLookup(uint8_t tableId, const char *name);


const char* getLookupString(uint8_t tableId, uint8_t idx, char *buf, uint8_t bufSize);

#ifdef __cplusplus
}
#endif

#endif
