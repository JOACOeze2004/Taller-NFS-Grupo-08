#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <unordered_map>
#include <memory>

enum class GameState {
    LOBBY,
    PLAYING,
    RESULTS
};

enum class SoundEffect {
    CRASH,
    DEATH,
    WIN,
    BRAKE,
    NITRO,
    RACE_START
};

class AudioManager {
private:
    std::unordered_map<GameState, Mix_Music*> music;
    std::unordered_map<SoundEffect, Mix_Chunk*> soundEffects;
    GameState currentState;
    bool initialized;
    int musicVolume;
    int effectsVolume;

public:
    AudioManager();
    ~AudioManager();

    bool initialize();
    void cleanup();

    // Gestión de música
    void changeState(GameState newState);
    void setMusicVolume(int volume); // 0-128
    void setEffectsVolume(int volume); // 0-128
    void pauseMusic();
    void resumeMusic();
    void stopMusic();

    // Efectos de sonido
    void playSoundEffect(SoundEffect effect);
    
    GameState getCurrentState() const { return currentState; }

private:
    void loadMusic();
    void loadSoundEffects();
    void playMusicForState(GameState state);
};

#endif // AUDIO_MANAGER_H