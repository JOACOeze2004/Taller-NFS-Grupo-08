#include "in_game.h"
#include "gameloop.h" 

InGame::InGame(Gameloop* _gameloop, float _duration): Phase(_gameloop, _duration), start_time(std::chrono::steady_clock::time_point{}), race_started(false) {}


bool InGame::should_continue(){
    return gameloop->is_running() && !gameloop->did_all_finish(); // agregar metodo q valide q todos los autos q estan vivos pasarlo la meta final.
}

void InGame::execute(ClientCommand& command) {
    gameloop->process_command(command);
}

void InGame::broadcast_phase() {
    gameloop->broadcast_in_game(this->get_time_remaining_ms());
}

void InGame::end() {
    gameloop->change_phase(std::make_unique<Workshop>(gameloop, 10.0f));
}

void InGame::update_phase() {
    if(!race_started){
        this->race_started = true;
        gameloop->start_race();
        start_time = std::chrono::steady_clock::now();
    }
    gameloop->update_positions();
    gameloop->update_race_state(); 
}

int InGame::get_time_remaining_ms() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
    return std::max(0, MAX_TIME_PER_RACE - static_cast<int>(elapsed));
}

