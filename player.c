#include "snake.h"
#include "player.h"
#include "tail.h"
#include "apple.h"

#define SPEED_START 5		// the starting speed
// #define SPEED_INC 1		// how many points it takes to speed up one more step

#define DEBUG_TAIL 0 		// set to 1 to print tail debugging info on screen

typedef struct {
    u8 x, y, d, speed;		// d is direction, has values like BTN_UP
    u8 nextd;
    Tail*tail;
    uint16_t score;
} Player;
Player _p;

static inline u8 opposite_d(u8 d) {
    switch (d) {
    case BTN_UP:    return BTN_DOWN;  break;
    case BTN_DOWN:  return BTN_UP;    break;
    case BTN_LEFT:  return BTN_RIGHT; break;
    case BTN_RIGHT: return BTN_LEFT;  break;
    }
    return 0;
}

bool player_tail_traverse(bool (*func)(u8 x, u8 y)) {
    Tail*tail = _p.tail;
    if (!tail)
	return false;

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
		if (out_of_bounds(i, j))
		    goto exit;
		if (func(i, j))
		    return true;
	    }
	}

	x0 = x1;
	y0 = y1;
	tail = tail->next;
    }
 exit:
    return false;    
}

void player_init() {
    _p.x = SCREEN_TILES_H/2;
    _p.y = SCREEN_TILES_V/2;
    _p.d = BTN_UP;
    _p.nextd = 0;
    _p.score = 0;
    _p.speed = SPEED_START;

    _p.tail = NULL;
}

void player_draw() {    
    SetTile(_p.x, _p.y, TILE_SNAKE);

    inline bool draw_tail_square(u8 x, u8 y) {
	SetTile(x, y, TILE_SNAKE);
	return false;
    }
    player_tail_traverse(draw_tail_square);

#if DEBUG_TAIL
    // ==== debugging ====
    Tail*tail = _p.tail;
    for (u8 i = 0; tail; i++, tail=tail->next) {
	PrintByte(2, i, i, false);
	Print(3, i, PSTR(": "));

	const u8 L1 = 5;

	switch (tail->d) {
	case BTN_UP:    Print(L1, i, PSTR("UP")); break;
	case BTN_DOWN:  Print(L1, i, PSTR("DN")); break;
	case BTN_LEFT:  Print(L1, i, PSTR("LT")); break;
	case BTN_RIGHT: Print(L1, i, PSTR("RT")); break;
	}

	PrintByte(L1+4, i, tail->l, false);
    }
#endif
}

void player_turn(u8 direction) {
    // ignore meaningless turns
    switch (direction) {
    case BTN_UP:
    case BTN_DOWN:
	switch (_p.d) {
	case BTN_UP:
	case BTN_DOWN:
	    return;
	}
	break;
    case BTN_LEFT:
    case BTN_RIGHT:
	switch (_p.d) {
	case BTN_LEFT:
	case BTN_RIGHT:
	    return;
	}
	break;
    }

    // queue this turn for update
    _p.nextd = direction;
}

// returns if we've died this update
bool player_update() {
    static bool should_extend_tail = false;
    
    // ========== deal with update speed
    static u8 counter = 0;
    // counter = (counter+1)%60;
    counter = (counter+1)%_p.speed;

    if (counter % _p.speed != 0)
    	return false;

    // ========== perform a turn if one is queued
    if (_p.nextd) {
	_p.d = _p.nextd;
	_p.nextd = 0;

	// add new tail at head
	if (_p.tail) {		// no need to add a new tail segment if there is no tail
	    Tail*newt = tail_new(opposite_d(_p.d), 0, _p.tail);
	    _p.tail = newt;
	}
    }

    // ========== actually move
    switch (_p.d) {
    case BTN_UP:    _p.y--; break;
    case BTN_DOWN:  _p.y++; break;
    case BTN_LEFT:  _p.x--; break;
    case BTN_RIGHT: _p.x++; break;
    }
    
    // ========== update tails
    // (update lengths of most recent and last ones, potentially remove last one)
    {
	Tail*head = _p.tail;
	Tail*last = tail_last(head);

	if (head) {
	    // adjust lengths
	    head->l++;
	    if (!should_extend_tail && last->l)
		last->l--;

	    // remove last tail if it's 0-length
	    if (!last->l) {
		Tail*curr = _p.tail;
		Tail*prev = curr;
		while (curr->next) {
		    prev = curr;
		    curr = curr->next;
		}
		tail_destroy_link(&prev->next);
	    }
	} else if (should_extend_tail) {
	    // there's no tail, but we need to extend it, so create new tail
	    _p.tail = tail_new(opposite_d(_p.d), 1, NULL);
	}
    }

    // ========== check if collided with own tail
    {
	static u8 count = 0;
	count = 0;
	inline bool collision_counter(u8 x, u8 y) {
	    if (x == _p.x && y == _p.y)
		count++;
	    return false;    
	}

	player_tail_traverse(collision_counter);
	if (count > 1)
	    return true;
    }

    // ========== check if collided with walls
    if (out_of_bounds(_p.x, _p.y))
	return true;

    // ========== eat any apple that exists
    if (apple_get_x() == _p.x && apple_get_y() == _p.y) {
	_p.score++;

	// TODO: modify speed based on score?
	// if (_p.score >= SPEED_INC*(SPEED_START-2))
	//     _p.speed = 3;
	// else
	//     _p.speed = SPEED_START - _p.score/SPEED_INC;
	
	apple_create();
	should_extend_tail = true;
    } else {
	should_extend_tail = false;
    }
    
    return false;
}

void player_destroy() {
    inline void tail_destroy(Tail**t) {
	if ((*t)->next)
	    tail_destroy(&((*t)->next));
	tail_destroy_link(t);
    }

    tail_destroy(&_p.tail);
}

inline uint16_t player_get_score() { return _p.score; }
