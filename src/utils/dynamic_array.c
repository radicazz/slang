#include "dynamic_array.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL_log.h>

void dynamic_array_init(dynamic_array_t* array) {
    assert(array != NULL);

    array->data = NULL;
    array->data_size = 0;
    array->size = 0;
    array->capacity = 0;
}

static bool dynamic_array_can_allocate(size_t data_size, size_t capacity) {
    return data_size <= (SIZE_MAX / capacity);
}

bool dynamic_array_create(dynamic_array_t* array, size_t data_size, size_t initial_capacity) {
    assert(array != NULL);
    assert(data_size > 0);
    assert(initial_capacity > 0);

    if (dynamic_array_can_allocate(data_size, initial_capacity) == false) {
        SDL_Log("Dynamic array allocation would overflow (element_size=%zu, capacity=%zu)", data_size,
                initial_capacity);
        return false;
    }

    // Initialize the dynamic array based on initial capacity and data size.
    array->data_size = data_size;
    array->size = 0;
    array->capacity = initial_capacity;
    array->data = malloc(data_size * initial_capacity);

    if (array->data == NULL) {
        SDL_Log("Failed to allocate dynamic array (element_size=%zu, capacity=%zu)", data_size, initial_capacity);
        array->data_size = 0;
        array->capacity = 0;
        return false;
    }

    return true;
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
static bool dynamic_array_resize(dynamic_array_t* array, size_t new_capacity) {
    assert(array != NULL);
    assert(new_capacity > 0);
    assert(new_capacity >= array->size);

    if (dynamic_array_can_allocate(array->data_size, new_capacity) == false) {
        SDL_Log("Dynamic array resize would overflow (element_size=%zu, capacity=%zu)", array->data_size, new_capacity);
        return false;
    }

    void* new_data = realloc(array->data, array->data_size * new_capacity);
    if (new_data == NULL) {
        SDL_Log("Failed to resize dynamic array to capacity %zu", new_capacity);
        return false;
    }

    array->data = new_data;
    array->capacity = new_capacity;
    return true;
}

bool dynamic_array_append(dynamic_array_t* array, const void* data) {
    assert(array != NULL);
    assert(data != NULL);

    // Resize the array if necessary.
    if (array->size >= array->capacity) {
        const size_t new_capacity = array->capacity > (SIZE_MAX / 2) ? SIZE_MAX : array->capacity * 2;
        if (new_capacity <= array->capacity || dynamic_array_resize(array, new_capacity) == false) {
            SDL_Log("Failed to grow dynamic array from capacity %zu", array->capacity);
            return false;
        }
    }

    memcpy((char*)array->data + (array->size * array->data_size), data, array->data_size);

    array->size++;
    return true;
}

void dynamic_array_remove(dynamic_array_t* array, size_t index) {
    assert(array != NULL);
    assert(index < array->size);

    if (index < array->size - 1) {
        memmove((char*)array->data + (index * array->data_size), (char*)array->data + ((index + 1) * array->data_size),
                (array->size - index - 1) * array->data_size);
    }

    array->size--;

    if (array->size > 0 && array->size < array->capacity / 4) {
        const size_t new_capacity = array->capacity / 2;
        if (new_capacity >= array->size) {
            (void)dynamic_array_resize(array, new_capacity);
        }
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
