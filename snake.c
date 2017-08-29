#include "snake.h"
#include "player.h"

#include "gen/tiles.inc"
#include "data/font-8x8-full.inc"

typedef enum {
    STATE_PLAYING, STATE_DIED
} GameState;
GameState _state = STATE_PLAYING;

typedef struct {
    u16 prev;			// Previous buttons that were held
    u16 down;			// Buttons that are held right now
    u16 prsd;			// Buttons that were pressed this frame
    u16 rlsd;			// Buttons that were released this frame
} ButtonData;
ButtonData _btn = {0, 0, 0, 0};

void button_update() {
    _btn.down = ReadJoypad(0);
    _btn.prsd = _btn.down & (_btn.down ^ _btn.prev);
    _btn.rlsd = _btn.prev & (_btn.down ^ _btn.prev);
    _btn.prev = _btn.down;
}

void draw_walls() {
    for (u8 i = 0; i < SCREEN_TILES_H; i++) {
	SetTile(i, 0, TILE_WALL);
	SetTile(i, 2, TILE_WALL);
	SetTile(i, SCREEN_TILES_V-1, TILE_WALL);
    }
	
    for (u8 i = 1; i < SCREEN_TILES_V; i++) {
	SetTile(0, i, TILE_WALL);
	SetTile(SCREEN_TILES_H-1, i, TILE_WALL);
    }    
}

void die() {
    player_destroy();
    _state = STATE_DIED;
    Print(SCREEN_TILES_H/2-5, SCREEN_TILES_V/2-3, PSTR("GAME OVER"));
    Print(SCREEN_TILES_H/2-6, SCREEN_TILES_V/2,   PSTR("PRESS START"));
}

int main() {
    SetFontTable(font);
    SetTileTable(TILETAB_TILES);
    ClearVram();
    
    player_init();

    while (true) {
    	WaitVsync(1);
    	button_update();

    	switch (_state) {
    	case STATE_PLAYING: {
    	    if (player_update()) {
    		die();
		break;
    	    }
	    
    	    ClearVram();
    	    draw_walls();
    	    player_draw();
    	} break;

    	case STATE_DIED: {
	    if (_btn.prsd & BTN_START) {
		player_init();
		_state = STATE_PLAYING;
	    }
    	} break;
    	}
    }
}
