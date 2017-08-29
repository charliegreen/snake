#ifndef SNAKE_H
#define SNAKE_H

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>

#define TILE_BG    0
#define TILE_SNAKE 1
#define TILE_WALL  2
#define TILE_APPLE 3

#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)>(b) ? (b) : (a))

#endif
