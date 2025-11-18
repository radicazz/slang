#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/dynamic_array.h"

typedef void (*test_fn_t)(void);

static int g_failures = 0;
static int g_tests_run = 0;
static const char* g_current_test = NULL;

#define TEST_ASSERT(cond)                                                                                \
    do {                                                                                                 \
        if (!(cond)) {                                                                                   \
            fprintf(stderr, "[  FAILED  ] %s: %s (%s:%d)\n", g_current_test, #cond, __FILE__, __LINE__); \
            ++g_failures;                                                                                \
            return;                                                                                      \
        }                                                                                                \
    } while (0)

#define TEST_ASSERT_EQUAL_SIZE_T(expected, actual) TEST_ASSERT((size_t)(expected) == (size_t)(actual))
#define TEST_ASSERT_EQUAL_INT(expected, actual) TEST_ASSERT((int)(expected) == (int)(actual))
#define TEST_ASSERT_NOT_NULL(ptr) TEST_ASSERT((ptr) != NULL)
#define TEST_ASSERT_NULL(ptr) TEST_ASSERT((ptr) == NULL)

static void dynamic_array_test_setup(dynamic_array_t* array) {
    dynamic_array_init(array);
    dynamic_array_create(array, sizeof(int), 4);
}

static void dynamic_array_test_teardown(dynamic_array_t* array) {
    dynamic_array_destroy(array);
}

static void test_create_and_destroy(void) {
    dynamic_array_t array;
    dynamic_array_init(&array);

    TEST_ASSERT_NULL(array.data);
    TEST_ASSERT_EQUAL_SIZE_T(0, array.size);
    TEST_ASSERT_EQUAL_SIZE_T(0, array.capacity);

    dynamic_array_create(&array, sizeof(int), 4);

    TEST_ASSERT_NOT_NULL(array.data);
    TEST_ASSERT_EQUAL_SIZE_T(0, array.size);
    TEST_ASSERT_EQUAL_SIZE_T(4, array.capacity);

    dynamic_array_destroy(&array);

    TEST_ASSERT_NULL(array.data);
    TEST_ASSERT_EQUAL_SIZE_T(0, array.size);
    TEST_ASSERT_EQUAL_SIZE_T(0, array.capacity);
}

static void test_append_and_get(void) {
    dynamic_array_t array;
    dynamic_array_test_setup(&array);

    for (int i = 0; i < 6; ++i) {
        dynamic_array_append(&array, &i);
        TEST_ASSERT_EQUAL_SIZE_T((size_t)(i + 1), array.size);
    }

    for (int i = 0; i < 6; ++i) {
        const int* value = (const int*)dynamic_array_get(&array, (size_t)i);
        TEST_ASSERT_NOT_NULL(value);
        TEST_ASSERT_EQUAL_INT(i, *value);
    }

    dynamic_array_test_teardown(&array);
}

static void test_set_and_get(void) {
    dynamic_array_t array;
    dynamic_array_test_setup(&array);

    for (int i = 0; i < 4; ++i) {
        int value = 0;
        dynamic_array_append(&array, &value);
    }

    for (int i = 0; i < 4; ++i) {
        dynamic_array_set(&array, (size_t)i, &i);
        const int* value = (const int*)dynamic_array_get(&array, (size_t)i);
        TEST_ASSERT_EQUAL_INT(i, *value);
    }

    dynamic_array_test_teardown(&array);
}

static void test_remove_compacts_array(void) {
    dynamic_array_t array;
    dynamic_array_test_setup(&array);

    for (int i = 0; i < 5; ++i) {
        dynamic_array_append(&array, &i);
    }

    dynamic_array_remove(&array, 2);

    TEST_ASSERT_EQUAL_SIZE_T(4, array.size);

    TEST_ASSERT_EQUAL_INT(0, *(int*)dynamic_array_get(&array, 0));
    TEST_ASSERT_EQUAL_INT(1, *(int*)dynamic_array_get(&array, 1));
    TEST_ASSERT_EQUAL_INT(3, *(int*)dynamic_array_get(&array, 2));
    TEST_ASSERT_EQUAL_INT(4, *(int*)dynamic_array_get(&array, 3));

    dynamic_array_test_teardown(&array);
}

static void test_resize_growth_and_shrink(void) {
    dynamic_array_t array;
    dynamic_array_init(&array);
    dynamic_array_create(&array, sizeof(int), 2);

    for (int i = 0; i < 5; ++i) {
        dynamic_array_append(&array, &i);
    }

    TEST_ASSERT_EQUAL_SIZE_T(5, array.size);
    TEST_ASSERT_EQUAL_SIZE_T(8, array.capacity);

    dynamic_array_remove(&array, 4);
    dynamic_array_remove(&array, 3);
    dynamic_array_remove(&array, 1);
    dynamic_array_remove(&array, 1);

    TEST_ASSERT_EQUAL_SIZE_T(1, array.size);
    TEST_ASSERT_EQUAL_SIZE_T(4, array.capacity);

    dynamic_array_test_teardown(&array);
}

static void test_is_empty(void) {
    dynamic_array_t array;
    dynamic_array_init(&array);
    TEST_ASSERT(dynamic_array_is_empty(&array));

    dynamic_array_create(&array, sizeof(int), 2);
    TEST_ASSERT(dynamic_array_is_empty(&array));

    int value = 42;
    dynamic_array_append(&array, &value);
    TEST_ASSERT(!dynamic_array_is_empty(&array));

    dynamic_array_remove(&array, 0);
    TEST_ASSERT(dynamic_array_is_empty(&array));

    dynamic_array_destroy(&array);
}

static void run_test(const char* name, test_fn_t fn) {
    g_current_test = name;
    const int failures_before = g_failures;
    fn();
    ++g_tests_run;
    if (g_failures == failures_before) {
        printf("[  PASSED  ] %s\n", name);
    }
}

int main(void) {
    printf("Running dynamic array unit tests...\n");

    run_test("test_create_and_destroy", test_create_and_destroy);
    run_test("test_append_and_get", test_append_and_get);
    run_test("test_set_and_get", test_set_and_get);
    run_test("test_remove_compacts_array", test_remove_compacts_array);
    run_test("test_resize_growth_and_shrink", test_resize_growth_and_shrink);
    run_test("test_is_empty", test_is_empty);

    if (g_failures > 0) {
        printf("%d/%d test(s) failed.\n", g_failures, g_tests_run);
        return EXIT_FAILURE;
    }

    printf("All %d dynamic array tests passed.\n", g_tests_run);
    return EXIT_SUCCESS;
}
