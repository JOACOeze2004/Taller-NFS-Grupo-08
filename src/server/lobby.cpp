#include "lobby.h"
#include "gameloop.h"

Lobby::Lobby(Gameloop* _gameloop, float _duration): Phase(_gameloop, _duration) {}


void Lobby::execute(ClientCommand& command) {
    gameloop->process_command(command);
    gameloop->handle_lobby_command(command);
}

bool Lobby::should_continue() { return gameloop->is_running() && !gameloop->is_game_already_started(); }

void Lobby::update_phase() {
    gameloop->update_positions(); 
}

void Lobby::broadcast_phase() {
    gameloop->broadcast_lobby();
}

void Lobby::end() {
    gameloop->change_phase(std::make_unique<InGame>(gameloop, MAX_TIME_PER_RACE));
}