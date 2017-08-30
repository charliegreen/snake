#include "snake.h"
#include "tail.h"

Tail*tail_new(u8 d, u8 l, Tail*next) {
    Tail*ret = (Tail*)malloc(sizeof(Tail));
    ret->d = d;
    ret->l = l;
    ret->next = next;
    return ret;
}

void tail_destroy_link(Tail**tail) {
    free(*tail);
    *tail = NULL;
}

Tail*tail_last(Tail*head) {
    if (!head)
	return NULL;
    while (head->next)
	head = head->next;
    return head;
}
