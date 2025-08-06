#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stddef.h>

int random_int(int min, int max);

/**
 * @brief 2D integer vector structure.
 */
typedef struct {
    int x;
    int y;
} ivec2_t;

void ivec2_init(ivec2_t* vec, int x, int y);

void ivec2_add(const ivec2_t* a, const ivec2_t* b, ivec2_t* result);
void ivec2_subtract(const ivec2_t* a, const ivec2_t* b, ivec2_t* result);

bool ivec2_equals(const ivec2_t* a, const ivec2_t* b);

/**
 * @brief Array structure that can dynamically grow.
 */
typedef struct {
    void* data;

    size_t data_size;
    size_t size;
    size_t capacity;
} dynamic_array_t;

void dynamic_array_create(dynamic_array_t* array, size_t data_size, size_t initial_capacity);
void dynamic_array_destroy(dynamic_array_t* array);

void dynamic_array_append(dynamic_array_t* array, const void* data);
void dynamic_array_remove(dynamic_array_t* array, size_t index);

void* dynamic_array_get(const dynamic_array_t* array, size_t index);

#endif  // UTILS_H
