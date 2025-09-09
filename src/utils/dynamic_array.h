#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    void* data;
    size_t data_size;
    size_t size;
    size_t capacity;
} dynamic_array_t;

void dynamic_array_init(dynamic_array_t* array);

void dynamic_array_create(dynamic_array_t* array, size_t data_size, size_t initial_capacity);
void dynamic_array_destroy(dynamic_array_t* array);

void dynamic_array_append(dynamic_array_t* array, const void* data);
void dynamic_array_remove(dynamic_array_t* array, size_t index);

void* dynamic_array_get(const dynamic_array_t* array, size_t index);
void dynamic_array_set(dynamic_array_t* array, size_t index, const void* data);

bool dynamic_array_is_empty(const dynamic_array_t* array);

#endif  // DYNAMIC_ARRAY_H
