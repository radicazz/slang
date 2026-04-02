#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "modules/config.h"
#include "modules/config_internal.h"

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
#define TEST_ASSERT_EQUAL_BOOL(expected, actual) TEST_ASSERT((expected) == (actual))
#define TEST_ASSERT_FLOAT_CLOSE(expected, actual) TEST_ASSERT(fabsf((expected) - (actual)) < 0.0001f)

static void test_defaults(void) {
    game_config_t config = {0};

    config_set_defaults(&config);

    TEST_ASSERT_EQUAL_SIZE_T(0, config.high_score);
    TEST_ASSERT_EQUAL_BOOL(false, config.mute);
    TEST_ASSERT_FLOAT_CLOSE(1.0f, config.volume);
    TEST_ASSERT_EQUAL_INT(CONFIG_RESUME_DELAY_DEFAULT, config.resume_delay_seconds);
}

static void test_parse_valid_buffer(void) {
    const char* contents = "high_score=12\nmute=yes\nvolume=0.375\nresume_delay=3\n";
    game_config_t config = {0};
    bool invalid = false;

    TEST_ASSERT(config_parse_buffer(contents, &config, &invalid));
    TEST_ASSERT_EQUAL_BOOL(false, invalid);
    TEST_ASSERT_EQUAL_SIZE_T(12, config.high_score);
    TEST_ASSERT_EQUAL_BOOL(true, config.mute);
    TEST_ASSERT_FLOAT_CLOSE(0.375f, config.volume);
    TEST_ASSERT_EQUAL_INT(3, config.resume_delay_seconds);
}

static void test_parse_invalid_line_marks_config_invalid(void) {
    const char* contents = "high_score=12\nthis-is-not-valid\n";
    game_config_t config = {0};
    bool invalid = false;

    TEST_ASSERT(config_parse_buffer(contents, &config, &invalid) == false);
    TEST_ASSERT_EQUAL_BOOL(true, invalid);
}

static void test_parse_invalid_value_marks_config_invalid(void) {
    const char* contents = "volume=loud\n";
    game_config_t config = {0};
    bool invalid = false;

    TEST_ASSERT(config_parse_buffer(contents, &config, &invalid) == false);
    TEST_ASSERT_EQUAL_BOOL(true, invalid);
}

static void test_normalize_clamps_values(void) {
    game_config_t config = {
        .high_score = 4,
        .mute = false,
        .volume = 2.5f,
        .resume_delay_seconds = -7,
    };

    config_normalize(&config);

    TEST_ASSERT_FLOAT_CLOSE(1.0f, config.volume);
    TEST_ASSERT_EQUAL_INT(CONFIG_RESUME_DELAY_MIN, config.resume_delay_seconds);
}

static void test_serialize_normalizes_and_round_trips(void) {
    game_config_t config = {
        .high_score = 18,
        .mute = true,
        .volume = -1.0f,
        .resume_delay_seconds = 99,
    };
    char buffer[128];
    game_config_t reparsed = {0};
    bool invalid = false;

    TEST_ASSERT(config_serialize(&config, buffer, sizeof(buffer)));
    TEST_ASSERT(config_parse_buffer(buffer, &reparsed, &invalid));
    TEST_ASSERT_EQUAL_BOOL(false, invalid);
    TEST_ASSERT_EQUAL_SIZE_T(18, reparsed.high_score);
    TEST_ASSERT_EQUAL_BOOL(true, reparsed.mute);
    TEST_ASSERT_FLOAT_CLOSE(0.0f, reparsed.volume);
    TEST_ASSERT_EQUAL_INT(CONFIG_RESUME_DELAY_MAX, reparsed.resume_delay_seconds);
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
    printf("Running config unit tests...\n");

    run_test("test_defaults", test_defaults);
    run_test("test_parse_valid_buffer", test_parse_valid_buffer);
    run_test("test_parse_invalid_line_marks_config_invalid", test_parse_invalid_line_marks_config_invalid);
    run_test("test_parse_invalid_value_marks_config_invalid", test_parse_invalid_value_marks_config_invalid);
    run_test("test_normalize_clamps_values", test_normalize_clamps_values);
    run_test("test_serialize_normalizes_and_round_trips", test_serialize_normalizes_and_round_trips);

    if (g_failures > 0) {
        printf("%d/%d test(s) failed.\n", g_failures, g_tests_run);
        return EXIT_FAILURE;
    }

    printf("All %d config tests passed.\n", g_tests_run);
    return EXIT_SUCCESS;
}
