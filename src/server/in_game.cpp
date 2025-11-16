#include "in_game.h"
#include "gameloop.h" 

InGame::InGame(Gameloop* _gameloop, float _duration): Phase(_gameloop, _duration), race_started(false) {}


bool InGame::should_continue(){
    if (!gameloop->has_active_players()) {
        std::cout << "[IN_GAME] No active players remaining, ending race" << std::endl;
        return false;
    }
    return gameloop->is_running() && !gameloop->did_all_finish(); // agregar metodo q valide q todos los autos q estan vivos pasarlo la meta final.
}

void InGame::execute(ClientCommand& command) {gameloop->process_command(command); }

void InGame::broadcast_phase(int time_ms) {gameloop->broadcast_in_game(time_ms);}

void InGame::end() {
    gameloop->increment_race_counter();

    if (gameloop->get_races_completed() >= MAX_RACES) {
        std::cout << "[IN_GAME] Max races completed (" << gameloop->get_races_completed()
                  << "/" << MAX_RACES << "), ending game" << std::endl;
        FinalScoreList final_results = gameloop->calculate_final_results();
        gameloop->broadcast_final_results(final_results);
        gameloop->stop();
        return;
    }

    std::cout << "[IN_GAME] Race " << gameloop->get_races_completed()
              << " completed, going to workshop" << std::endl;
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

