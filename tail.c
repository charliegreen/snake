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

bool tail_traverse(Tail*tail, u8 x0, u8 y0, bool (*func)(u8 x, u8 y)) {
    if (!tail)
	return false;

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

// // ==================== I know, this is messy, but it wouldn't let me do a closure ;_;
// u8 _helper_x = 0, _helper_y = 0;
// static inline bool helper(u8 x1, u8 y1) {
//     return _helper_x == x1 && _helper_y == y1;
// }

// bool tail_collides(Tail*tail, u8 x0, u8 y0, u8 x, u8 y) {
//     _helper_x = x;
//     _helper_y = y;
//     return tail_traverse(tail, x0, y0, helper);
// }
