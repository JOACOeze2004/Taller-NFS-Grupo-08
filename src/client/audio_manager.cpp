#include "audio_manager.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

AudioManager::AudioManager() 
    : currentState(GameState::LOBBY)
    , initialized(false)
    , musicVolume(64)
    , effectsVolume(64) {
}

AudioManager::~AudioManager() {
    cleanup();
}

bool AudioManager::initialize() {
    int devnull = open("/dev/null", O_WRONLY);
    if (devnull != -1) {
        dup2(devnull, STDERR_FILENO);
        close(devnull);
    }
    
    const char* drivers[] = {"pulseaudio", "alsa", "dsp", "disk", "dummy", nullptr};
    bool audio_initialized = false;
    
    for (int i = 0; drivers[i] != nullptr && !audio_initialized; i++) {
        if (SDL_AudioInit(drivers[i]) == 0) {
            audio_initialized = true;
            break;
        }
    }
    
    if (!audio_initialized) {
        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
            return true;
        }
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return true;
    }

    Mix_AllocateChannels(16);
    initialized = true;

    loadMusic();
    loadSoundEffects();

    Mix_VolumeMusic(musicVolume);
    playMusicForState(currentState);

    return true;
}

void AudioManager::loadMusic() {
    music[GameState::LOBBY] = Mix_LoadMUS("../assets/music/lobby.mp3");
    music[GameState::PLAYING] = Mix_LoadMUS("../assets/music/game.mp3");
    music[GameState::RESULTS] = Mix_LoadMUS("../assets/music/results.mp3");

    for (const auto& [state, mus] : music) {
        if (!mus) {
            std::cerr << "No se pudo cargar música: " << Mix_GetError() << std::endl;
        }
    }
}

void AudioManager::loadSoundEffects() {
    soundEffects[SoundEffect::CRASH] = Mix_LoadWAV("../assets/sounds/crashing.mp3");
    soundEffects[SoundEffect::DEATH] = Mix_LoadWAV("../assets/sounds/death.mp3");
    soundEffects[SoundEffect::WIN] = Mix_LoadWAV("../assets/sounds/reach_finish.mp3");
    soundEffects[SoundEffect::BRAKE] = Mix_LoadWAV("../assets/sounds/braking.mp3");
    soundEffects[SoundEffect::NITRO] = Mix_LoadWAV("../assets/sounds/nitro.mp3");
    soundEffects[SoundEffect::RACE_START] = Mix_LoadWAV("../assets/sounds/countdown.mp3");
    
    for (auto& [effect, chunk] : soundEffects) {
        if (!chunk) {
            std::cerr << "No se pudo cargar efecto: " << Mix_GetError() << std::endl;
        } else {
            Mix_VolumeChunk(chunk, effectsVolume);
        }
    }
}

void AudioManager::playMusicForState(GameState state) {
    if (!initialized) return;

    auto it = music.find(state);
    if (it != music.end() && it->second) {
        Mix_PlayMusic(it->second, -1);
    }
}

void AudioManager::changeState(GameState newState) {
    if (currentState == newState) return;

    currentState = newState;
    Mix_FadeOutMusic(500);
    
    SDL_Delay(600);
    playMusicForState(newState);
}

void AudioManager::setMusicVolume(int volume) {
    musicVolume = std::min(128, std::max(0, volume));
    Mix_VolumeMusic(musicVolume);
}

void AudioManager::setEffectsVolume(int volume) {
    effectsVolume = std::min(128, std::max(0, volume));
    for (auto& [effect, chunk] : soundEffects) {
        if (chunk) {
            Mix_VolumeChunk(chunk, effectsVolume);
        }
    }
}

void AudioManager::pauseMusic() {
    if (initialized) {
        Mix_PauseMusic();
    }
}

void AudioManager::resumeMusic() {
    if (initialized) {
        Mix_ResumeMusic();
    }
}

void AudioManager::stopMusic() {
    if (initialized) {
        Mix_HaltMusic();
    }
}

void AudioManager::playSoundEffect(SoundEffect effect) {
    if (!initialized) return;

    auto it = soundEffects.find(effect);
    if (it != soundEffects.end() && it->second) {
        Mix_PlayChannel(-1, it->second, 0);
    }
}

void AudioManager::cleanup() {
    if (!initialized) return;

    for (auto& [state, mus] : music) {
        if (mus) {
            Mix_FreeMusic(mus);
        }
    }
    music.clear();

    for (auto& [effect, chunk] : soundEffects) {
        if (chunk) {
            Mix_FreeChunk(chunk);
        }
    }
    soundEffects.clear();

    Mix_CloseAudio();
    initialized = false;
}