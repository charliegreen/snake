#ifndef PLAYER_H
#define PLAYER_H

#include "tail.h"

typedef struct {
    u8 x, y, d, speed;		// d is direction, has values like BTN_UP
    u8 nextd;
    Tail*tail;
} Player;

void player_init();
void player_draw();
void player_turn(u8 direction);
bool player_update();
void player_destroy();

// Call FUNC on all locations player's tail crosses. FUNC returns true if it should break
// early. TAIL_TRAVERSE returns true if FUNC broke early.
bool player_tail_traverse(bool (*func)(u8 x, u8 y));

#endif
