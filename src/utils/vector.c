#include "vector.h"

#include <SDL3/SDL.h>

#include <assert.h>

static int random_int(int max) {
    assert(max > 0);
    return SDL_rand(max) + 1;
}

void vector2i_set(vector2i_t* vec, int x, int y) {
    assert(vec != NULL);

    vec->x = x;
    vec->y = y;
}

void vector2i_add(const vector2i_t* a, const vector2i_t* b, vector2i_t* result) {
    assert(a != NULL);
    assert(b != NULL);
    assert(result != NULL);

    result->x = a->x + b->x;
    result->y = a->y + b->y;
}

void vector2i_subtract(const vector2i_t* a, const vector2i_t* b, vector2i_t* result) {
    assert(a != NULL);
    assert(b != NULL);
    assert(result != NULL);

    result->x = a->x - b->x;
    result->y = a->y - b->y;
}

bool vector2i_equals(const vector2i_t* a, const vector2i_t* b) {
    assert(a != NULL);
    assert(b != NULL);

    return (a->x == b->x) && (a->y == b->y);
}

void vector2i_random(vector2i_t* vec, int max_x, int max_y) {
    assert(vec != NULL);
    assert(max_x > 0);
    assert(max_y > 0);

    vec->x = random_int(max_x);
    vec->y = random_int(max_y);
}
