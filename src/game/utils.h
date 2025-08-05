#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

typedef struct {
    int x;
    int y;
} point_t;

void point_init(point_t* point, int x, int y);

void point_add(const point_t* a, const point_t* b, point_t* result);
void point_subtract(const point_t* a, const point_t* b, point_t* result);

bool point_equals(const point_t* a, const point_t* b);

#endif  // UTILS_H
