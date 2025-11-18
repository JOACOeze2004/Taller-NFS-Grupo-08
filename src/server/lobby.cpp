#include "lobby.h"
#include "gameloop.h"

Lobby::Lobby(Gameloop* _gameloop, float _duration): Phase(_gameloop, _duration) {}


void Lobby::execute(ClientCommand& command) {
    gameloop->process_command(command);
    gameloop->handle_lobby_command(command);
}

bool Lobby::should_continue() { return gameloop->is_running() && !gameloop->is_game_already_started(); }

void Lobby::update_phase() {gameloop->update_positions(); }

void Lobby::broadcast_phase(int time_ms) {gameloop->broadcast_lobby(time_ms);}

void Lobby::end() { gameloop->change_phase(std::make_unique<InGame>(gameloop, MAX_TIME_PER_RACE + RACE_START_TIME)); }

State Lobby::get_current_phase_state() const { return IN_LOBBY; }