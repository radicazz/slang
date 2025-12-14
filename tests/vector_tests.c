#include <stdio.h>
#include <stdlib.h>

#include <SDL3/SDL.h>

#include "utils/vector.h"

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

#define TEST_ASSERT_EQUAL_INT(expected, actual) TEST_ASSERT((int)(expected) == (int)(actual))
#define TEST_ASSERT_TRUE(value) TEST_ASSERT((value) == true)
#define TEST_ASSERT_FALSE(value) TEST_ASSERT((value) == false)

static void test_set(void) {
    vector2i_t v = {0};
    vector2i_set(&v, 3, -4);
    TEST_ASSERT_EQUAL_INT(3, v.x);
    TEST_ASSERT_EQUAL_INT(-4, v.y);
}

static void test_add(void) {
    const vector2i_t a = {.x = 5, .y = -2};
    const vector2i_t b = {.x = -7, .y = 10};
    vector2i_t r = {0};

    vector2i_add(&a, &b, &r);
    TEST_ASSERT_EQUAL_INT(-2, r.x);
    TEST_ASSERT_EQUAL_INT(8, r.y);
}

static void test_subtract(void) {
    const vector2i_t a = {.x = 5, .y = -2};
    const vector2i_t b = {.x = -7, .y = 10};
    vector2i_t r = {0};

    vector2i_subtract(&a, &b, &r);
    TEST_ASSERT_EQUAL_INT(12, r.x);
    TEST_ASSERT_EQUAL_INT(-12, r.y);
}

static void test_equals(void) {
    const vector2i_t a = {.x = 1, .y = 2};
    const vector2i_t b = {.x = 1, .y = 2};
    const vector2i_t c = {.x = 2, .y = 1};

    TEST_ASSERT_TRUE(vector2i_equals(&a, &b));
    TEST_ASSERT_FALSE(vector2i_equals(&a, &c));
}

static void test_random_in_range(void) {
    SDL_srand(123u);

    for (int i = 0; i < 1000; ++i) {
        vector2i_t v = {0};
        vector2i_random(&v, 7, 9);
        TEST_ASSERT(v.x >= 1 && v.x <= 7);
        TEST_ASSERT(v.y >= 1 && v.y <= 9);
    }
}

static void test_random_max_one_always_one(void) {
    SDL_srand(999u);

    for (int i = 0; i < 50; ++i) {
        vector2i_t v = {0};
        vector2i_random(&v, 1, 1);
        TEST_ASSERT_EQUAL_INT(1, v.x);
        TEST_ASSERT_EQUAL_INT(1, v.y);
    }
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
    printf("Running vector unit tests...\n");

    run_test("test_set", test_set);
    run_test("test_add", test_add);
    run_test("test_subtract", test_subtract);
    run_test("test_equals", test_equals);
    run_test("test_random_in_range", test_random_in_range);
    run_test("test_random_max_one_always_one", test_random_max_one_always_one);

    if (g_failures > 0) {
        printf("%d/%d test(s) failed.\n", g_failures, g_tests_run);
        return EXIT_FAILURE;
    }

    printf("All %d vector tests passed.\n", g_tests_run);
    return EXIT_SUCCESS;
}
