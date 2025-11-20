#include "phase.h"
#include "gameloop.h"


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

int Phase::get_time_remaining_ms(const float base_time) const {
    if (this->get_current_phase_state() == IN_LOBBY){
        return 1;
    }    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
    return std::max(0, static_cast<int>(base_time) - static_cast<int>(elapsed));
}

int Phase::get_time() const {
    if (this->get_current_phase_state() == IN_LOBBY){
        return 1;
    }
    auto now = std::chrono::steady_clock::now();
    return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count()/1000);
}

Phase::~Phase() {}