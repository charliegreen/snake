#include "snake.h"
#include "player.h"
#include "apple.h"
#include "score.h"

#include "gen/tiles.inc"
#include "data/font-8x8-full.inc"

typedef enum {
    STATE_PLAYING, STATE_DIED, STATE_MENU, STATE_SCORES, STATE_ENTER_NAME
} GameState;
GameState _state = STATE_MENU;

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

bool out_of_bounds(u8 x, u8 y) {
    if (x <= 0 || x >= SCREEN_TILES_H-1 ||
	y <= 2 || y >= SCREEN_TILES_V-1)
	return true;
    return false;
}

int main() {
    GetPrngNumber(GetTrueRandomSeed());
    SetFontTable(font);
    SetTileTable(TILETAB_TILES);
    ClearVram();

    static bool _state_first_update = true;
    inline void switch_state(u8 state) {
	_state = state;
	_state_first_update = true;
    }
    inline bool state_first_updatep() {
	if (_state_first_update) {
	    _state_first_update = false;
	    return true;
	}
	return false;
    }
    
    while (true) {
    	WaitVsync(1);
    	button_update();

#if 1
    	switch (_state) {
    	case STATE_PLAYING: {
	    if (state_first_updatep()) {
		player_init();
		apple_create();
	    }
	    
	    static bool paused = false;
	    if (_btn.prsd & BTN_START) {
		if (!paused) {
		    Print(SCREEN_TILES_H/2-3, SCREEN_TILES_V/2-1, PSTR("PAUSED"));
		}
		paused = !paused;
	    }
	    if (paused)
		continue;
	    
	    if      (_btn.prsd & BTN_UP)    player_turn(BTN_UP);
	    else if (_btn.prsd & BTN_DOWN)  player_turn(BTN_DOWN);
	    else if (_btn.prsd & BTN_LEFT)  player_turn(BTN_LEFT);
	    else if (_btn.prsd & BTN_RIGHT) player_turn(BTN_RIGHT);

    	    if (player_update()) {
    		switch_state(STATE_DIED);
		break;
    	    }
	    
    	    ClearVram();
    	    draw_walls();
    	    player_draw();
	    apple_draw();

	    Print(SCREEN_TILES_H-6-7, 1, PSTR("SCORE: "));
	    PrintInt(SCREEN_TILES_H-2, 1, player_get_score(), false);
    	} break;

    	case STATE_DIED: {
	    if (state_first_updatep()) {
		player_destroy();
		Print(SCREEN_TILES_H/2-5, SCREEN_TILES_V/2-3, PSTR("GAME OVER"));
		// Print(SCREEN_TILES_H/2-6, SCREEN_TILES_V/2,   PSTR("PRESS START"));

		Print(SCREEN_TILES_H/2-10, SCREEN_TILES_V/2+2, PSTR("PRESS A TO CONTINUE"));
		Print(SCREEN_TILES_H/2-10, SCREEN_TILES_V/2+4, PSTR("PRESS START FOR MENU"));
	    }
	    
	    if (_btn.prsd & BTN_A)
		switch_state(STATE_PLAYING);
	    
	    if (_btn.prsd & BTN_START)
		switch_state(STATE_MENU);
    	} break;

	case STATE_MENU: {
	    const static u8 L = 8, U = 14;
	    if (state_first_updatep()) {
		ClearVram();
		draw_walls();
		Print(SCREEN_TILES_H/2-3, 8, PSTR("SNAKE!"));

		Print(L, U+0, PSTR("START GAME"));
		Print(L, U+2, PSTR("VIEW HIGH SCORES"));
	    }

	    static u8 selection = 0;

	    FontFill(L-2, U, 1, 6, ' ');
	    PrintChar(L-2, U+2*selection, '>');

	    if (_btn.prsd & BTN_DOWN || _btn.prsd & BTN_RIGHT)
		selection = (selection+1)%2;
	    if (_btn.prsd & BTN_UP || _btn.prsd & BTN_LEFT)
		selection = (2+selection-1)%2;

	    if (_btn.prsd & BTN_A || _btn.prsd & BTN_START)
		switch_state(selection == 0 ? STATE_PLAYING : STATE_SCORES);
	    
	} break;

	case STATE_SCORES: {
	    if (state_first_updatep()) {
		ClearVram();
		draw_walls();
		load_scores();
		draw_scores();
	    }

	    if (_btn.prsd & BTN_START || _btn.prsd & BTN_A)
		switch_state(STATE_MENU);
	} break;

	case STATE_ENTER_NAME: {
	    // TODO
	} break;
    	}
#endif
    }
}
