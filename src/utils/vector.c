#include "vector.h"

#include <SDL3/SDL.h>

#include <assert.h>

static int random_int(int max) {
    assert(max > 0);
    return SDL_rand(max) + 1;
}

void ivec2_set(ivec2_t* vec, int x, int y) {
    assert(vec != NULL);

    vec->x = x;
    vec->y = y;
}

void ivec2_add(const ivec2_t* a, const ivec2_t* b, ivec2_t* result) {
    assert(a != NULL);
    assert(b != NULL);
    assert(result != NULL);

    result->x = a->x + b->x;
    result->y = a->y + b->y;
}

void ivec2_subtract(const ivec2_t* a, const ivec2_t* b, ivec2_t* result) {
    assert(a != NULL);
    assert(b != NULL);
    assert(result != NULL);

    result->x = a->x - b->x;
    result->y = a->y - b->y;
}

bool ivec2_equals(const ivec2_t* a, const ivec2_t* b) {
    assert(a != NULL);
    assert(b != NULL);

    return (a->x == b->x) && (a->y == b->y);
}

void ivec2_random(ivec2_t* vec, int max_x, int max_y) {
    assert(vec != NULL);
    assert(max_x > 0);
    assert(max_y > 0);

    vec->x = random_int(max_x);
    vec->y = random_int(max_y);
}
