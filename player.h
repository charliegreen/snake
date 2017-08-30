#ifndef PLAYER_H
#define PLAYER_H

void player_init();
void player_draw();
void player_turn(u8 direction);
bool player_update();
void player_destroy();

// Call FUNC on all locations player's tail crosses. FUNC returns true if it should break
// early. TAIL_TRAVERSE returns true if FUNC broke early.
bool player_tail_traverse(bool (*func)(u8 x, u8 y));

uint16_t player_get_score();

#endif
