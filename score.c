#include "snake.h"
#include "score.h"

#include <kernel.h>

#define EEPROM_BLOCK_ID 19
#define DATA_SIZE 30

#define TABLE_ENTRIES 3
#define TABLE_NAME_LEN 8

typedef struct {
    unsigned char name[TABLE_NAME_LEN];	// remember to null-terminate when printing!
    uint16_t score;
} ScoreTableEntry;

typedef struct {
    ScoreTableEntry entries[TABLE_ENTRIES];
} ScoreTable;

ScoreTable*_table = NULL;

static void mcopy(u8*dst, u8*src, uint16_t len) {
    while (len) {
	*dst = *src;
	len--;
	dst++;
	src++;
    }
}

// static void hexdump(u8 x0, u8 y0, u8*p) {
//     for (u8 i = 0; i < DATA_SIZE; i++) {
// 	u8 x = 3*(i%8)+x0, y = y0+(i/8);
// 	PrintHexByte(x, y, *(p + i));
//     }    
// }

static void score_initialize() {
    mcopy((u8*)&_table->entries[0].name, (u8*)"charlie\0", 8);
    _table->entries[0].score = 0x53;
    
    mcopy((u8*)&_table->entries[1].name, (u8*)"charlie\0", 8);
    _table->entries[1].score = 0x43;
    
    mcopy((u8*)&_table->entries[2].name, (u8*)"else\0\0\0\0", 8);
    _table->entries[2].score = 0x15;

    save_scores();
}

void load_scores() {
    struct EepromBlockStruct block;
    _table = malloc(DATA_SIZE);

    if (EepromReadBlock(EEPROM_BLOCK_ID, &block)) {
	// if block doesn't exist, create it and reread it
	score_initialize();
	EepromReadBlock(EEPROM_BLOCK_ID, &block);
    }
    
    mcopy((u8*)_table, (u8*)block.data, DATA_SIZE);    
    // hexdump(2, 2, (u8*)&block.data);
}

void save_scores() {
    struct EepromBlockStruct block;
    block.id = EEPROM_BLOCK_ID;
    mcopy((u8*)&block.data, (u8*)_table, 32);
    EepromWriteBlock(&block);
}

// draw highscore table
void draw_scores() {
    // ClearVram();

    if (!_table) {
	Print(0, 0, PSTR("Err: scores not loaded"));
	return;
    }

    static const u8 L = 6, R = L+TABLE_NAME_LEN+4+5, U = 10;

    Print(SCREEN_TILES_H/2-6, U-3, PSTR("HIGH SCORES!"));
    
    for (u8 i = 0; i < TABLE_ENTRIES; i++) {
    	PrintRam(L, U+2*i, _table->entries[i].name);
    	PrintInt(R, U+2*i, _table->entries[i].score, false);
    }
}

bool score_highp(uint16_t score) { return score > _table->entries[0].score; }

// ================================================================================
// ================================================================================
#if 0
// #include <sdBase.h>
#include <petitfatfs/pff.h>

#define TABLE_ENTRIES 8
#define TABLE_NAME_LEN 8

// ID for "Direct Access" method for _HISCORE.DAT; See:
// http://uzebox.org/wiki/SD_Save_Sector_Reservation_List
#define SD_BLOCK_ID 18

typedef struct {
    char name[TABLE_NAME_LEN];	// remember to null-terminate when printing!
    uint16_t score;
} ScoreTableEntry;

typedef struct {
    ScoreTableEntry entries[TABLE_ENTRIES];
} ScoreTable;

ScoreTable*_table = NULL;

void load_scores() {
    _table = malloc(sizeof(ScoreTable));

    for (uint8_t i=0; i<sizeof(ScoreTable); i++)
    	*((uint8_t*)(_table)+i) = 0;

    FATFS fs;
    FRESULT res = pf_mount(&fs);
    PrintByte(3, 0, res, false);
    
    res = pf_open(PSTR("_HISCORE.DAT"));
    PrintByte(3, 1, res, false);

    WORD read = 0;
    res = pf_read(_table, sizeof(ScoreTable), &read);
    PrintByte(3, 2, res, false);
    
    // uint32_t sector_start = sdCardFindFileFirstSector(PSTR("_HISCORE.DAT"));

    // if (sector_start == 0) {
    // 	Print(0, 0, PSTR("ERR: _HISCORE.DAT not found"));
    // 	while (true);
    // }

    // sdCardCueSectorAddress(sector_start + SD_BLOCK_ID);
    // sdCardDirectReadSimple((uint8_t*)_table, sizeof(ScoreTable));
}

void save_scores() {

}

#endif
