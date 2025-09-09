#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>

typedef struct {
    int x;
    int y;
} ivec2_t;

void ivec2_set(ivec2_t* vec, int x, int y);

void ivec2_add(const ivec2_t* a, const ivec2_t* b, ivec2_t* result);
void ivec2_subtract(const ivec2_t* a, const ivec2_t* b, ivec2_t* result);

bool ivec2_equals(const ivec2_t* a, const ivec2_t* b);

void ivec2_random(ivec2_t* vec, int max_x, int max_y);

#endif  // VECTOR_H
