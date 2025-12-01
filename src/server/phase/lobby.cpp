#include "lobby.h"
#include "../gameloop.h"
#include "countdown.h"

Lobby::Lobby(Gameloop* _gameloop, float _duration): Phase(_gameloop, _duration) {}

void Lobby::execute(ClientCommand& command) {
    gameloop->process_command(command);
    gameloop->handle_lobby_command(command);
}

bool Lobby::should_continue() { return gameloop->is_running() && !gameloop->is_game_already_started(); }

void Lobby::update(int time_ms) {
    gameloop->update_positions(); 
    gameloop->broadcast_lobby(time_ms);
}

void Lobby::end() { gameloop->change_phase(std::make_unique<Countdown>(gameloop, RACE_START_TIME)); }


State Lobby::get_current_phase_state() const { return IN_LOBBY; }