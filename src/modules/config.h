#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    size_t high_score;
    bool mute;
    float volume;
} game_config_t;

void config_set_defaults(game_config_t* config);
bool config_load(game_config_t* config);
bool config_save(const game_config_t* config);

#endif  // CONFIG_H
