#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <SDL3/SDL.h>

static const char* k_config_filename = "config.ini";

static void config_clamp(game_config_t* config) {
    SDL_assert(config != NULL);

    if (config->volume < 0.0f) {
        config->volume = 0.0f;
    } else if (config->volume > 1.0f) {
        config->volume = 1.0f;
    }

    if (config->resume_delay_seconds < CONFIG_RESUME_DELAY_MIN) {
        config->resume_delay_seconds = CONFIG_RESUME_DELAY_MIN;
    } else if (config->resume_delay_seconds > CONFIG_RESUME_DELAY_MAX) {
        config->resume_delay_seconds = CONFIG_RESUME_DELAY_MAX;
    }
}

void config_set_defaults(game_config_t* config) {
    SDL_assert(config != NULL);

    config->high_score = 0;
    config->mute = false;
    config->volume = 1.0f;
    config->resume_delay_seconds = CONFIG_RESUME_DELAY_DEFAULT;
}

static bool config_build_path_from_base(const char* base_path, char* out_path, size_t path_size) {
    SDL_assert(base_path != NULL);
    SDL_assert(out_path != NULL);
    SDL_assert(path_size > 0);

    const int written = snprintf(out_path, path_size, "%s%s", base_path, k_config_filename);
    if (written < 0 || (size_t)written >= path_size) {
        SDL_Log("Config path is too long");
        return false;
    }

    return true;
}

static bool config_get_paths(char* out_primary, size_t primary_size, char* out_fallback, size_t fallback_size) {
    SDL_assert(out_primary != NULL);
    SDL_assert(out_fallback != NULL);
    SDL_assert(primary_size > 0);
    SDL_assert(fallback_size > 0);

    char* base_path = SDL_GetBasePath();
    const char* base_path_to_use = base_path;
    if (base_path_to_use == NULL || base_path_to_use[0] == '\0') {
        SDL_Log("Failed to get base path for config: %s", SDL_GetError());
        base_path_to_use = "./";
    }

    bool success = config_build_path_from_base(base_path_to_use, out_primary, primary_size);
    if (success == true) {
        success = config_build_path_from_base("./", out_fallback, fallback_size);
    }

    if (base_path != NULL) {
        SDL_free(base_path);
    }

    return success;
}

static bool config_parse_bool(const char* value, bool* out_value) {
    SDL_assert(value != NULL);
    SDL_assert(out_value != NULL);

    if (SDL_strcasecmp(value, "true") == 0 || SDL_strcasecmp(value, "1") == 0 ||
        SDL_strcasecmp(value, "yes") == 0) {
        *out_value = true;
        return true;
    }

    if (SDL_strcasecmp(value, "false") == 0 || SDL_strcasecmp(value, "0") == 0 ||
        SDL_strcasecmp(value, "no") == 0) {
        *out_value = false;
        return true;
    }

    return false;
}

static bool config_parse_float(const char* value, float* out_value) {
    SDL_assert(value != NULL);
    SDL_assert(out_value != NULL);

    char* end_ptr = NULL;
    const float parsed = strtof(value, &end_ptr);
    if (end_ptr == value || *end_ptr != '\0') {
        return false;
    }

    *out_value = parsed;
    return true;
}

static bool config_parse_size(const char* value, size_t* out_value) {
    SDL_assert(value != NULL);
    SDL_assert(out_value != NULL);

    char* end_ptr = NULL;
    const unsigned long long parsed = strtoull(value, &end_ptr, 10);
    if (end_ptr == value || *end_ptr != '\0') {
        return false;
    }

    *out_value = (size_t)parsed;
    return true;
}

static bool config_write_file(const char* path, const game_config_t* config) {
    SDL_assert(path != NULL);
    SDL_assert(config != NULL);

    char temp_path[512];
    const int temp_written = snprintf(temp_path, sizeof(temp_path), "%s.tmp", path);
    if (temp_written < 0 || (size_t)temp_written >= sizeof(temp_path)) {
        SDL_Log("Config temp path is too long");
        return false;
    }

    FILE* file = fopen(temp_path, "wb");
    if (file == NULL) {
        SDL_Log("Failed to open config for writing: %s", temp_path);
        return false;
    }

    const int written = fprintf(file, "high_score=%zu\nmute=%d\nvolume=%.3f\nresume_delay=%d\n", config->high_score,
                                config->mute ? 1 : 0, config->volume, config->resume_delay_seconds);
    if (written <= 0) {
        SDL_Log("Failed to write config contents");
        fclose(file);
        return false;
    }

    if (fclose(file) != 0) {
        SDL_Log("Failed to close config after writing");
        return false;
    }

    if (rename(temp_path, path) != 0) {
        SDL_Log("Failed to replace config file");
        if (remove(path) != 0 || rename(temp_path, path) != 0) {
            return false;
        }
    }

    SDL_Log("Config saved to %s", path);
    return true;
}

bool config_save(const game_config_t* config) {
    SDL_assert(config != NULL);

    char primary_path[512];
    char fallback_path[512];
    if (config_get_paths(primary_path, sizeof(primary_path), fallback_path, sizeof(fallback_path)) == false) {
        return false;
    }

    if (config_write_file(primary_path, config) == true) {
        return true;
    }

    if (SDL_strcmp(primary_path, fallback_path) != 0) {
        SDL_Log("Falling back to config path: %s", fallback_path);
        return config_write_file(fallback_path, config);
    }

    return false;
}

static bool config_read_file(const char* path, game_config_t* config, bool* out_invalid) {
    SDL_assert(path != NULL);
    SDL_assert(config != NULL);
    SDL_assert(out_invalid != NULL);

    *out_invalid = false;
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        return false;
    }

    char line[256];
    while (fgets(line, (int)sizeof(line), file) != NULL) {
        char* newline = strchr(line, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }

        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        char* separator = strchr(line, '=');
        if (separator == NULL) {
            SDL_Log("Invalid config line (missing '='): %s", line);
            *out_invalid = true;
            break;
        }

        *separator = '\0';
        const char* key = line;
        const char* value = separator + 1;

        if (SDL_strcasecmp(key, "high_score") == 0) {
            size_t parsed = 0;
            if (config_parse_size(value, &parsed) == false) {
                SDL_Log("Invalid high_score value: %s", value);
                *out_invalid = true;
                break;
            }
            config->high_score = parsed;
        } else if (SDL_strcasecmp(key, "mute") == 0) {
            bool parsed = false;
            if (config_parse_bool(value, &parsed) == false) {
                SDL_Log("Invalid mute value: %s", value);
                *out_invalid = true;
                break;
            }
            config->mute = parsed;
        } else if (SDL_strcasecmp(key, "volume") == 0) {
            float parsed = 0.0f;
            if (config_parse_float(value, &parsed) == false) {
                SDL_Log("Invalid volume value: %s", value);
                *out_invalid = true;
                break;
            }
            config->volume = parsed;
        } else if (SDL_strcasecmp(key, "resume_delay") == 0) {
            size_t parsed = 0;
            if (config_parse_size(value, &parsed) == false) {
                SDL_Log("Invalid resume_delay value: %s", value);
                *out_invalid = true;
                break;
            }
            config->resume_delay_seconds = (int)parsed;
        }
    }

    fclose(file);
    return *out_invalid == false;
}

bool config_load(game_config_t* config) {
    SDL_assert(config != NULL);

    config_set_defaults(config);

    char primary_path[512];
    char fallback_path[512];
    if (config_get_paths(primary_path, sizeof(primary_path), fallback_path, sizeof(fallback_path)) == false) {
        return false;
    }

    bool invalid = false;
    const char* loaded_path = primary_path;
    if (config_read_file(primary_path, config, &invalid) == false) {
        if (invalid == true) {
            config_set_defaults(config);
            return config_save(config);
        }

        if (SDL_strcmp(primary_path, fallback_path) != 0 &&
            config_read_file(fallback_path, config, &invalid) == true) {
            SDL_Log("Loaded config from fallback path: %s", fallback_path);
            loaded_path = fallback_path;
        } else if (invalid == true) {
            config_set_defaults(config);
            return config_save(config);
        } else {
            SDL_Log("Config file missing, creating defaults: %s", primary_path);
            return config_save(config);
        }
    }

    config_clamp(config);
    if (config_save(config) == false) {
        SDL_Log("Failed to rewrite config with normalized values");
        return false;
    }

    SDL_Log("Config loaded from %s", loaded_path);
    return true;
}
