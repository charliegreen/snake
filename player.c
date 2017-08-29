#include "snake.h"
#include "player.h"

typedef struct Tail {
    u8 d, l;
    struct Tail*next;
} Tail;

struct {
    u8 x, y, d, speed;		// d is direction, has values like BTN_UP
    Tail*tail;
} _p;

// Tail t3 = { BTN_LEFT, 9, NULL };
// Tail t2 = { BTN_UP, 7, &t3 };
// Tail t1 = { BTN_RIGHT, 5, &t2 };
// Tail t0 = { BTN_DOWN, 3, &t1 };

void player_init() {
    _p.x = SCREEN_TILES_H/2;
    _p.y = SCREEN_TILES_V/2;
    _p.d = BTN_UP;
    _p.speed = 30;
    _p.tail = NULL;
}

void player_draw() {
    SetTile(_p.x, _p.y, TILE_SNAKE);
    Tail*tail = _p.tail;

    u8 x0 = _p.x, y0 = _p.y;
    
    while (tail) {
	u8 x1 = x0, y1 = y0;
	switch (tail->d) {
	case BTN_UP:    y1 = y0-tail->l; break;
	case BTN_DOWN:  y1 = y0+tail->l; break;
	case BTN_LEFT:  x1 = x0-tail->l; break;
	case BTN_RIGHT: x1 = x0+tail->l; break;
	}

	for (u8 i = MIN(x0, x1); i <= MAX(x0, x1); i++) {
	    for (u8 j = MIN(y0, y1); j <= MAX(y0, y1); j++) {
		SetTile(i, j, TILE_SNAKE);
	    }
	}

	x0 = x1;
	y0 = y1;
	tail = tail->next;
    }
}

void player_turn(u8 direction) {
    
}

// returns if we've died this update
bool player_update() {
    static u8 counter = 0;
    counter = (counter+1)%60;

    if (counter % _p.speed == 0) {
	switch (_p.d) {
	case BTN_UP:    _p.y--; break;
	case BTN_DOWN:  _p.y++; break;
	case BTN_LEFT:  _p.x--; break;
	case BTN_RIGHT: _p.x++; break;
	}
    }

    // TODO: update tails (update lengths of most recent and last ones, potentially remove last one)

    // TODO: check if collided with own tail

    if (_p.x == 0 || _p.x == SCREEN_TILES_H-1 ||
	_p.y == 2 || _p.y == SCREEN_TILES_V-1)
	return true;

    return false;
}

void player_destroy() {
    
}
