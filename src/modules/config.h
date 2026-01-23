#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stddef.h>

#define CONFIG_RESUME_DELAY_MIN 0
#define CONFIG_RESUME_DELAY_MAX 3
#define CONFIG_RESUME_DELAY_DEFAULT 2

typedef struct {
    size_t high_score;
    bool mute;
    float volume;
    int resume_delay_seconds;
} game_config_t;

void config_set_defaults(game_config_t* config);
bool config_load(game_config_t* config);
bool config_save(const game_config_t* config);

#endif  // CONFIG_H
