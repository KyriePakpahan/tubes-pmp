#include "lookup.h"
#include "validation.h"
#include <string.h>
#include <stdlib.h>

#ifdef ARDUINO
#include <Arduino.h>
#include <avr/pgmspace.h>
#else

#define strcmp_P strcmp
#endif

static char* custom_strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* p = (char*)malloc(len);
    if (p) {
        memcpy(p, s, len);
    }
    return p;
}

// static tables in PROGMEM with clean spacing
const char kat_0[] PROGMEM = "Mikrokontroler";
const char kat_1[] PROGMEM = "SBC";
const char kat_2[] PROGMEM = "Sensor";
const char kat_3[] PROGMEM = "Aktuator";
const char kat_4[] PROGMEM = "Display";
const char kat_5[] PROGMEM = "Alat Ukur";
const char kat_6[] PROGMEM = "Diskrit";
const char kat_7[] PROGMEM = "Saklar";
const char kat_8[] PROGMEM = "IC";
const char kat_9[] PROGMEM = "Power";
const char kat_10[] PROGMEM = "Pasif";
const char kat_11[] PROGMEM = "Proteksi";

const char* const kategoriTable[] PROGMEM = {
    kat_0, kat_1, kat_2, kat_3, kat_4, kat_5,
    kat_6, kat_7, kat_8, kat_9, kat_10, kat_11
};
#define KATEGORI_STATIC_COUNT 12

const char lok_0[] PROGMEM = "Lab EE";
const char lok_1[] PROGMEM = "Lab Robot";
const char lok_2[] PROGMEM = "Lab Fisika";

const char* const lokasiTable[] PROGMEM = {
    lok_0, lok_1, lok_2
};
#define LOKASI_STATIC_COUNT 3

const char pem_0[] PROGMEM = "Pak Nana";
const char pem_1[] PROGMEM = "Pak Mervin";
const char pem_2[] PROGMEM = "Pak Budi";
const char pem_3[] PROGMEM = "Pak Yusuf";
const char pem_4[] PROGMEM = "Pak Infal";

const char* const pemilikTable[] PROGMEM = {
    pem_0, pem_1, pem_2, pem_3, pem_4
};
#define PEMILIK_STATIC_COUNT 5

const char pic_0[] PROGMEM = "Azka";
const char pic_1[] PROGMEM = "Kenneth";
const char pic_2[] PROGMEM = "Kuri";
const char pic_3[] PROGMEM = "Kyrie";

const char* const picTable[] PROGMEM = {
    pic_0, pic_1, pic_2, pic_3
};
#define PIC_STATIC_COUNT 5

// dynamic SRAM pools for custom items
static char* customKategori[MAX_CUSTOM];
static char* customLokasi[MAX_CUSTOM];
static char* customPemilik[MAX_CUSTOM];
static char* customPic[MAX_CUSTOM];

static uint8_t customKategoriCount = 0;
static uint8_t customLokasiCount = 0;
static uint8_t customPemilikCount = 0;
static uint8_t customPicCount = 0;

void initLookup(void) {
    customKategoriCount = 0;
    customLokasiCount = 0;
    customPemilikCount = 0;
    customPicCount = 0;
    for (int i = 0; i < MAX_CUSTOM; i++) {
        customKategori[i] = NULL;
        customLokasi[i] = NULL;
        customPemilik[i] = NULL;
        customPic[i] = NULL;
    }
}

void freeLookup(void) {
    for (int i = 0; i < MAX_CUSTOM; i++) {
        if (customKategori[i]) { free(customKategori[i]); customKategori[i] = NULL; }
        if (customLokasi[i]) { free(customLokasi[i]); customLokasi[i] = NULL; }
        if (customPemilik[i]) { free(customPemilik[i]); customPemilik[i] = NULL; }
        if (customPic[i]) { free(customPic[i]); customPic[i] = NULL; }
    }
    customKategoriCount = 0;
    customLokasiCount = 0;
    customPemilikCount = 0;
    customPicCount = 0;
}

static int findInStaticTable(const char* const* table, uint8_t count, const char* name) {
    for (uint8_t i = 0; i < count; i++) {
        const char* p = (const char*)pgm_read_ptr(&(table[i]));
        if (strcmp_P(name, p) == 0) {
            return i;
        }
    }
    return -1;
}

static int findInDynamicTable(char* const* table, uint8_t count, const char* name) {
    for (uint8_t i = 0; i < count; i++) {
        if (table[i] != NULL && strcmp(table[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

uint8_t registerLookup(uint8_t tableId, const char *name) {
    if (name == NULL || name[0] == '\0') return LOOKUP_INVALID;

    char temp[32];
    strncpy(temp, name, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    normalisasiNama(temp);

    const char* const* staticTable = NULL;
    uint8_t staticCount = 0;
    char** dynamicTable = NULL;
    uint8_t *dynamicCountPtr = NULL;

    switch (tableId) {
        case TABLE_KATEGORI:
            staticTable = kategoriTable;
            staticCount = KATEGORI_STATIC_COUNT;
            dynamicTable = customKategori;
            dynamicCountPtr = &customKategoriCount;
            break;
        case TABLE_LOKASI:
            staticTable = lokasiTable;
            staticCount = LOKASI_STATIC_COUNT;
            dynamicTable = customLokasi;
            dynamicCountPtr = &customLokasiCount;
            break;
        case TABLE_PEMILIK:
            staticTable = pemilikTable;
            staticCount = PEMILIK_STATIC_COUNT;
            dynamicTable = customPemilik;
            dynamicCountPtr = &customPemilikCount;
            break;
        case TABLE_PIC:
            staticTable = picTable;
            staticCount = PIC_STATIC_COUNT;
            dynamicTable = customPic;
            dynamicCountPtr = &customPicCount;
            break;
        default:
            return LOOKUP_INVALID;
    }

    int idx = findInStaticTable(staticTable, staticCount, temp);
    if (idx >= 0) {
        return (uint8_t)idx;
    }

    idx = findInDynamicTable(dynamicTable, *dynamicCountPtr, temp);
    if (idx >= 0) {
        return 128 + (uint8_t)idx;
    }

    if (*dynamicCountPtr < MAX_CUSTOM) {
        uint8_t newIdx = *dynamicCountPtr;
        dynamicTable[newIdx] = custom_strdup(temp);
        if (dynamicTable[newIdx] == NULL) {
            return LOOKUP_INVALID;
        }
        (*dynamicCountPtr)++;
        return 128 + newIdx;
    }

    return LOOKUP_INVALID;
}

const char* getLookupString(uint8_t tableId, uint8_t idx, char *buf, uint8_t bufSize) {
    const char* const* staticTable = NULL;
    uint8_t staticCount = 0;
    char** dynamicTable = NULL;
    uint8_t dynamicCount = 0;

    switch (tableId) {
        case TABLE_KATEGORI:
            staticTable = kategoriTable;
            staticCount = KATEGORI_STATIC_COUNT;
            dynamicTable = customKategori;
            dynamicCount = customKategoriCount;
            break;
        case TABLE_LOKASI:
            staticTable = lokasiTable;
            staticCount = LOKASI_STATIC_COUNT;
            dynamicTable = customLokasi;
            dynamicCount = customLokasiCount;
            break;
        case TABLE_PEMILIK:
            staticTable = pemilikTable;
            staticCount = PEMILIK_STATIC_COUNT;
            dynamicTable = customPemilik;
            dynamicCount = customPemilikCount;
            break;
        case TABLE_PIC:
            staticTable = picTable;
            staticCount = PIC_STATIC_COUNT;
            dynamicTable = customPic;
            dynamicCount = customPicCount;
            break;
        default:
            if (buf && bufSize > 0) buf[0] = '\0';
            return buf;
    }

    if (idx < 128) {
        if (idx < staticCount) {
            const char* p = (const char*)pgm_read_ptr(&(staticTable[idx]));
            if (buf && bufSize > 0) {
                #ifdef ARDUINO
                strncpy_P(buf, p, bufSize - 1);
                buf[bufSize - 1] = '\0';
                #else
                strncpy(buf, p, bufSize - 1);
                buf[bufSize - 1] = '\0';
                #endif
                return buf;
            }
            return p;
        }
    } else if (idx >= 128 && idx < 128 + MAX_CUSTOM) {
        uint8_t sramIdx = idx - 128;
        if (sramIdx < dynamicCount && dynamicTable[sramIdx] != NULL) {
            if (buf && bufSize > 0) {
                strncpy(buf, dynamicTable[sramIdx], bufSize - 1);
                buf[bufSize - 1] = '\0';
                return buf;
            }
            return dynamicTable[sramIdx];
        }
    }

    if (buf && bufSize > 0) {
        strncpy(buf, "Lain-lain", bufSize - 1);
        buf[bufSize - 1] = '\0';
        return buf;
    }
    return "Lain-lain";
}
