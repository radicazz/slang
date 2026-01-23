#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>
#include <stddef.h>
#include <SDL3/SDL_audio.h>

typedef enum {
    SOUND_EAT_FOOD,
    SOUND_COUNT  // Must be last
} sound_id_t;

typedef struct {
    uint8_t* buffer;
    size_t length;
    SDL_AudioSpec spec;
} sound_data_t;

typedef struct {
    SDL_AudioStream* stream;
    sound_data_t sounds[SOUND_COUNT];
    bool is_initialized;
    bool is_muted;
    float volume;
} audio_manager_t;

bool audio_manager_create(audio_manager_t* manager);
void audio_manager_destroy(audio_manager_t* manager);

bool audio_manager_load_sound(audio_manager_t* manager, sound_id_t id, const char* filepath);
bool audio_manager_play_sound(audio_manager_t* manager, sound_id_t id);
bool audio_manager_set_volume(audio_manager_t* manager, float volume);
bool audio_manager_set_muted(audio_manager_t* manager, bool muted);
float audio_manager_get_volume(const audio_manager_t* manager);
bool audio_manager_is_muted(const audio_manager_t* manager);

#endif  // AUDIO_H
