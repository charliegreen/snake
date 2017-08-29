#ifndef PLAYER_H
#define PLAYER_H

void player_init();
void player_draw();
void player_turn(u8 direction);
bool player_update();
void player_destroy();

#endif
