#include "snake.h"
#include "score.h"

#include <kernel.h>

#define EEPROM_BLOCK_ID 19
#define DATA_SIZE 30

#define TABLE_ENTRIES 3
#define TABLE_NAME_LEN 8

typedef struct {
    unsigned char name[TABLE_NAME_LEN];
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
    mcopy((u8*)&_table->entries[0].name, (u8*)"bytsqid\0", 8);
    _table->entries[0].score = 10;
    
    mcopy((u8*)&_table->entries[1].name, (u8*)"charlie\0", 8);
    _table->entries[1].score = 2;
    
    mcopy((u8*)&_table->entries[2].name, (u8*)"else\0\0\0\0", 8);
    _table->entries[2].score = 0;

    save_scores();
}

void load_scores() {
    struct EepromBlockStruct block;
    _table = malloc(DATA_SIZE);

    if (EepromReadBlock(EEPROM_BLOCK_ID, &block)) {
	// if block doesn't exist, create it
	score_initialize();
    }
    
    mcopy((u8*)_table, (u8*)block.data, DATA_SIZE);    
}

void save_scores() {
    struct EepromBlockStruct block;
    block.id = EEPROM_BLOCK_ID;
    mcopy((u8*)&block.data, (u8*)_table, DATA_SIZE);

    Print(1, 1, PSTR("Saving..."));
    char r = EepromWriteBlock(&block);
    if (r) {
	Print(9, 1, PSTR("ERR:"));
    	PrintByte(16, 1, r, false);
	while (true);		// so error stays on screen and user sees it
    } else {
	Print(9, 1, PSTR("Saved!"));
    }
}

// draw highscore table
void draw_scores() {
    if (!_table) {
	Print(1, 1, PSTR("Err: scores not loaded"));
	return;
    }

    static const u8 L = 6, R = L+TABLE_NAME_LEN+4+5, U = 10;

    Print(SCREEN_TILES_H/2-6, U-3, PSTR("HIGH SCORES!"));
    
    for (u8 i = 0; i < TABLE_ENTRIES; i++) {
    	PrintRam(L, U+2*i, _table->entries[i].name);
    	PrintInt(R, U+2*i, _table->entries[i].score, false);
    }

    Print(SCREEN_TILES_H/2-6, SCREEN_TILES_V-8, PSTR("PRESS START"));
}

void score_add(unsigned char*name, uint16_t score) {
    // find what place in the high score table we are
    u8 idx = TABLE_ENTRIES;
    for (u8 i = TABLE_ENTRIES; i>0; i--)
	if (_table->entries[i-1].score < score)
	    idx = i-1;
    
    // return if we have no place
    if (idx == TABLE_ENTRIES)
	return;

    // move lower scores down
    for (u8 i = TABLE_ENTRIES-1; i>0 && i>idx; i--) {
	mcopy((u8*)&_table->entries[i], (u8*)&_table->entries[i-1], TABLE_NAME_LEN+2);
    }

    // copy over this score
    mcopy((u8*)_table->entries[idx].name, (u8*)name, 8);
    _table->entries[idx].score = score;
}

bool score_highp(uint16_t score) { return score > _table->entries[TABLE_ENTRIES-1].score; }
