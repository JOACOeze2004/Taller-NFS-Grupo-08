#include "in_game.h"
#include "gameloop.h" 

InGame::InGame(Gameloop* _gameloop, float _duration): Phase(_gameloop, _duration), race_started(false) {}


bool InGame::should_continue(){
    return gameloop->is_running() && !gameloop->did_all_finish(); // agregar metodo q valide q todos los autos q estan vivos pasarlo la meta final.
}

void InGame::execute(ClientCommand& command) {gameloop->process_command(command); }

void InGame::broadcast_phase(int time_ms) {gameloop->broadcast_in_game(time_ms);}

void InGame::end() {
    std::cout<< "creando fase de workshop" << std::endl;
    gameloop->change_phase(std::make_unique<Workshop>(gameloop, MAX_TIME_PER_WORKSHOP));
}

void InGame::update_phase() {
    if(!race_started){
        this->race_started = true;
        gameloop->start_race();
    }
    gameloop->update_positions();
    gameloop->update_race_state(); 
}

State InGame::get_current_phase_state() const { return IN_RACE; }

