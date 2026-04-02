#ifndef CONFIG_INTERNAL_H
#define CONFIG_INTERNAL_H

#include <stdbool.h>
#include <stddef.h>

#include "config.h"

void config_normalize(game_config_t* config);
bool config_parse_buffer(const char* contents, game_config_t* config, bool* out_invalid);
bool config_serialize(const game_config_t* config, char* out_buffer, size_t buffer_size);

#endif  // CONFIG_INTERNAL_H
