#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>

typedef struct {
    int x;
    int y;
} vector2i_t;

void vector2i_set(vector2i_t* vec, int x, int y);

void vector2i_add(const vector2i_t* a, const vector2i_t* b, vector2i_t* result);
void vector2i_subtract(const vector2i_t* a, const vector2i_t* b, vector2i_t* result);

bool vector2i_equals(const vector2i_t* a, const vector2i_t* b);

void vector2i_random(vector2i_t* vec, int max_x, int max_y);

#endif  // VECTOR_H
