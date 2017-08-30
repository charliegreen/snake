#ifndef TAIL_H
#define TAIL_H

typedef struct Tail {
    u8 d, l;
    struct Tail*next;
} Tail;

Tail*tail_new(u8 d, u8 l, Tail*next);
void tail_destroy_link(Tail**tail);
Tail*tail_last(Tail*head);

#endif
