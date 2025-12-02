#include "phase.h"
#include "../gameloop.h"

Phase::Phase(Gameloop* _gameloop, float _duration) : gameloop(_gameloop), duration(_duration) {}

void Phase::run() { 
    auto rate = std::chrono::milliseconds(16);
    auto t1 = std::chrono::steady_clock::now();
    start_time = std::chrono::steady_clock::now();

    while (this->should_continue() && this->get_time_remaining_ms(duration) > 0) {
        ClientCommand cmd;
        while (gameloop->try_pop(cmd)) {
            this->execute(cmd);
        }
        update(this->get_time_remaining_ms(duration));
        
        auto t2 = std::chrono::steady_clock::now();
        auto rest = rate - std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

        if (rest < std::chrono::milliseconds(0)) {
            auto behind = -rest;
            rest = rate - behind % rate;
            auto lost = behind + rest;
            t1 += lost;
        }

        std::this_thread::sleep_for(rest);
        t1 += rate;
    }
    this->end();
}

bool Phase::is_in_lobby() const { return this->get_current_phase_state() == IN_LOBBY; }

int Phase::get_elapsed_ms() const {
    auto now = std::chrono::steady_clock::now();
    return static_cast<int>( std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count());
}

int Phase::calculate_remaining_time(float base_time, int elapsed_ms) const { return std::max(0, static_cast<int>(base_time) - elapsed_ms); }

int Phase::calculate_remaining_time_with_penalty(float base_time, int penalty_ms, int elapsed_ms) const {
    int available_time = static_cast<int>(base_time) - penalty_ms;
    return std::max(0, available_time - elapsed_ms);
}

int Phase::get_time_remaining_ms(const float base_time) const {
    if (is_in_lobby()) {
        return 1;
    }
    int elapsed = get_elapsed_ms();
    return calculate_remaining_time(base_time, elapsed);
}

int Phase::get_time() const {
    if (is_in_lobby()) {
        return 1;
    }
    return get_elapsed_ms() / 1000;
}

int Phase::get_time_remaining_ms(const float base_time, int player_id) const {
    if (is_in_lobby()) {
        return 1;
    }
    int elapsed = get_elapsed_ms();
    int penalty_ms = gameloop->get_upgrade_penalty(player_id) * 1000;
    return calculate_remaining_time_with_penalty(base_time, penalty_ms, elapsed);
}

Phase::~Phase() {}