#include "dynamic_array.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void dynamic_array_init(dynamic_array_t* array) {
    assert(array != NULL);

    array->data = NULL;
    array->data_size = 0;
    array->size = 0;
    array->capacity = 0;
}

void dynamic_array_create(dynamic_array_t* array, size_t data_size, size_t initial_capacity) {
    assert(array != NULL);
    assert(data_size > 0);
    assert(initial_capacity > 0);

    // Initialize the dynamic array based on initial capacity and data size.
    array->data_size = data_size;
    array->size = 0;
    array->capacity = initial_capacity;
    array->data = malloc(data_size * initial_capacity);

    assert(array->data != NULL);
}

void dynamic_array_destroy(dynamic_array_t* array) {
    assert(array != NULL);

    if (array->data != NULL) {
        free(array->data);

        // Reset the structure.
        array->data = NULL;
        array->data_size = 0;
        array->size = 0;
        array->capacity = 0;
    }
}

/**
 * @brief Resize the dynamic array to a new capacity.
 *
 * Used internally when appending or removing elements to ensure the array can hold more data.
 *
 * @param array The dynamic array to resize.
 * @param new_capacity The new capacity for the dynamic array.
 */
static void dynamic_array_resize(dynamic_array_t* array, size_t new_capacity) {
    assert(array != NULL);
    assert(new_capacity > array->capacity);

    void* new_data = realloc(array->data, array->data_size * new_capacity);
    assert(new_data != NULL);

    array->data = new_data;
    array->capacity = new_capacity;
}

void dynamic_array_append(dynamic_array_t* array, const void* data) {
    assert(array != NULL);
    assert(data != NULL);

    // Resize the array if necessary.
    if (array->size >= array->capacity) {
        dynamic_array_resize(array, array->capacity * 2);
    }

    memcpy((char*)array->data + (array->size * array->data_size), data, array->data_size);

    array->size++;
}

void dynamic_array_remove(dynamic_array_t* array, size_t index) {
    assert(array != NULL);
    assert(index < array->size);

    if (index < array->size - 1) {
        memmove((char*)array->data + (index * array->data_size), (char*)array->data + ((index + 1) * array->data_size),
                (array->size - index - 1) * array->data_size);
    }

    array->size--;

    if (array->size < array->capacity / 4) {
        dynamic_array_resize(array, array->capacity / 2);
    }
}

void* dynamic_array_get(const dynamic_array_t* array, size_t index) {
    assert(array != NULL);
    assert(index < array->size);

    return (char*)array->data + (index * array->data_size);
}

void dynamic_array_set(dynamic_array_t* array, size_t index, const void* data) {
    assert(array != NULL);
    assert(index < array->size);
    assert(data != NULL);

    memcpy((char*)array->data + (index * array->data_size), data, array->data_size);
}

bool dynamic_array_is_empty(const dynamic_array_t* array) {
    assert(array != NULL);
    return array->size == 0;
}
