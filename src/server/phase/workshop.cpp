#include "workshop.h"
#include "../../common/constants.h"
#include "../gameloop.h"
#include "countdown.h"

Workshop::Workshop(Gameloop* _gameloop, float _duration)
    : Phase(_gameloop, _duration) { }

void Workshop::execute(ClientCommand& command) { gameloop->process_command(command); }

bool Workshop::should_continue() {
    if (!gameloop->has_active_players()) {
        return false;
    }
    return gameloop->is_running();
}

void Workshop::update(int time_ms) { gameloop->broadcast_workshop(time_ms); } 

void Workshop::end() {
    if (!gameloop->has_active_players()) {
        gameloop->stop();
        return;
    }
    gameloop->change_phase(std::make_unique<Countdown>(gameloop, RACE_START_TIME));
}

State Workshop::get_current_phase_state() const { return IN_WORK_SHOP; }