#ifndef TAIL_H
#define TAIL_H

typedef struct Tail {
    u8 d, l;
    struct Tail*next;
} Tail;

Tail*tail_new(u8 d, u8 l, Tail*next);
void tail_destroy_link(Tail**tail);
Tail*tail_last(Tail*head);

// Call FUNC on all locations LIST crosses. FUNC returns true if it should break
// early. TAIL_TRAVERSE returns true if FUNC broke early.
bool tail_traverse(Tail*tail, u8 x0, u8 y0, bool (*func)(u8 x, u8 y));

// bool tail_collides(Tail*tail, u8 x0, u8 y0, u8 x, u8 y);

#endif
