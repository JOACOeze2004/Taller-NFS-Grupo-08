#include "workshop.h"

#include "../common/constants.h"
#include "gameloop.h"

Workshop::Workshop(Gameloop* _gameloop, float _duration)
    : Phase(_gameloop, _duration) { }

void Workshop::execute(ClientCommand& command) { gameloop->process_command(command); }

bool Workshop::should_continue() {
    if (!gameloop->has_active_players()) {
        std::cout << "[WORKSHOP] No active players remaining, ending game" << std::endl;
        return false;
    }
    return gameloop->is_running();
}

void Workshop::update(int time_ms) { gameloop->broadcast_workshop(time_ms); } 

void Workshop::end() {
    if (!gameloop->has_active_players()) {
        std::cout << "[WORKSHOP] No players left, ending game" << std::endl;
        gameloop->stop();
        return;
    }
    gameloop->reset_race();
    std::cout << "[WORKSHOP] Starting next race (Race "
              << (gameloop->get_races_completed() + 1) << ")" << std::endl;
    gameloop->change_phase(std::make_unique<InGame>(gameloop, MAX_TIME_PER_RACE + RACE_START_TIME));
}

State Workshop::get_current_phase_state() const { return IN_WORK_SHOP; }