#include "audio.h"

#include <string.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>

bool audio_manager_create(audio_manager_t* manager) {
    SDL_assert(manager != NULL);

    memset(manager, 0, sizeof(*manager));

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) == false) {
        SDL_Log("Failed to initialize SDL audio subsystem: %s", SDL_GetError());
        return false;
    }

    const SDL_AudioSpec spec = {.format = SDL_AUDIO_F32, .channels = 2, .freq = 44100};

    manager->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (manager->stream == NULL) {
        SDL_Log("Failed to open audio device stream: %s", SDL_GetError());
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return false;
    }

    if (SDL_ResumeAudioStreamDevice(manager->stream) == false) {
        SDL_Log("Failed to resume audio stream device: %s", SDL_GetError());
        SDL_DestroyAudioStream(manager->stream);
        manager->stream = NULL;
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return false;
    }

    manager->is_initialized = true;
    return true;
}

void audio_manager_destroy(audio_manager_t* manager) {
    SDL_assert(manager != NULL);

    if (manager->is_initialized == false) {
        return;
    }

    for (size_t i = 0; i < SOUND_COUNT; ++i) {
        if (manager->sounds[i].buffer != NULL) {
            SDL_free(manager->sounds[i].buffer);
            manager->sounds[i].buffer = NULL;
        }
        manager->sounds[i].length = 0;
    }

    if (manager->stream != NULL) {
        SDL_DestroyAudioStream(manager->stream);
        manager->stream = NULL;
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    manager->is_initialized = false;
}

bool audio_manager_load_sound(audio_manager_t* manager, sound_id_t id, const char* filepath) {
    SDL_assert(manager != NULL);
    SDL_assert(filepath != NULL);

    if (manager->is_initialized == false) {
        SDL_Log("Audio manager not initialized, cannot load sound: %s", filepath);
        return false;
    }

    if (id >= SOUND_COUNT) {
        SDL_Log("Invalid sound ID: %d", id);
        return false;
    }

    if (manager->sounds[id].buffer != NULL) {
        SDL_Log("Sound already loaded for ID %d, skipping: %s", id, filepath);
        return true;
    }

    SDL_AudioSpec spec;
    uint8_t* buffer = NULL;
    Uint32 length = 0;

    if (SDL_LoadWAV(filepath, &spec, &buffer, &length) == false) {
        SDL_Log("Failed to load audio file '%s': %s", filepath, SDL_GetError());
        return false;
    }

    SDL_AudioSpec target_spec;
    if (SDL_GetAudioStreamFormat(manager->stream, &target_spec, NULL) == false) {
        SDL_Log("Failed to get audio stream format: %s", SDL_GetError());
        SDL_free(buffer);
        return false;
    }

    uint8_t* converted_buffer = NULL;
    int converted_length = 0;

    if (SDL_ConvertAudioSamples(&spec, buffer, (int)length, &target_spec, &converted_buffer, &converted_length) ==
        false) {
        SDL_Log("Failed to convert audio samples for '%s': %s", filepath, SDL_GetError());
        SDL_free(buffer);
        return false;
    }

    SDL_free(buffer);

    manager->sounds[id].buffer = converted_buffer;
    manager->sounds[id].length = (size_t)converted_length;
    manager->sounds[id].spec = target_spec;

    SDL_Log("Successfully loaded sound: %s (ID: %d, %d bytes)", filepath, id, converted_length);
    return true;
}

bool audio_manager_play_sound(audio_manager_t* manager, sound_id_t id) {
    SDL_assert(manager != NULL);

    if (manager->is_initialized == false) {
        return false;
    }

    if (id >= SOUND_COUNT) {
        SDL_Log("Invalid sound ID: %d", id);
        return false;
    }

    // TODO: Implement sound playback
    return false;
}
