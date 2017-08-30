#include "snake.h"
#include "tail.h"
#include "player.h"
#include "apple.h"

struct {
    u8 x;
    u8 y;
} _current = { 0, 0 };

void apple_create() {
    inline bool bad_apple(u8 x, u8 y) {
	return _current.x == x && _current.y == y;
    }
    
    while (true) {
	_current.x = (u8)GetPrngNumber(0) % (SCREEN_TILES_H-2) + 1;
	_current.y = (u8)GetPrngNumber(0) % (SCREEN_TILES_V-4) + 3;

	if (!player_tail_traverse(bad_apple))
	    break;
    }
}

void apple_draw() {
    if (_current.x != 0 || _current.y != 0) {
	SetTile(_current.x, _current.y, TILE_APPLE);
    }
}

inline u8 apple_get_x() { return _current.x; }
inline u8 apple_get_y() { return _current.y; }
