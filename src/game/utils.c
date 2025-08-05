#include "utils.h"

#include <assert.h>

void point_init(point_t* point, int x, int y) {
    assert(point != NULL);

    point->x = x;
    point->y = y;
}

void point_add(const point_t* a, const point_t* b, point_t* result) {
    assert(a != NULL);
    assert(b != NULL);
    assert(result != NULL);

    result->x = a->x + b->x;
    result->y = a->y + b->y;
}

void point_subtract(const point_t* a, const point_t* b, point_t* result) {
    assert(a != NULL);
    assert(b != NULL);
    assert(result != NULL);

    result->x = a->x - b->x;
    result->y = a->y - b->y;
}

bool point_equals(const point_t* a, const point_t* b) {
    assert(a != NULL);
    assert(b != NULL);

    return (a->x == b->x) && (a->y == b->y);
}
