#include "countdown.h"
#include "../gameloop.h"
#include "../../common/constants.h"

Countdown::Countdown(Gameloop* gameloop, float duration)
    : Phase(gameloop, duration) { }

void Countdown::execute(ClientCommand& command) {
    gameloop->process_command(command);
}

bool Countdown::should_continue() {
    return gameloop->is_running() && gameloop->has_active_players();
}

void Countdown::update(int time_ms) {
    gameloop->broadcast_countdown(time_ms);
}

void Countdown::end() {
    gameloop->start_race();
    gameloop->change_phase(std::make_unique<InGame>(gameloop, MAX_TIME_PER_RACE));
}

State Countdown::get_current_phase_state() const {
    return IN_COUNTDOWN;
}