#ifndef SCORE_H
#define SCORE_H

void load_scores();
void draw_scores();
void save_scores();
bool score_highp(uint16_t score);
void score_add(unsigned char*name, uint16_t score);

#endif
