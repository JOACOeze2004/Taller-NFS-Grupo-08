#include "phase.h"
#include "gameloop.h"


Phase::Phase(Gameloop* _gameloop, float _duration) : gameloop(_gameloop), duration(_duration) {}

void Phase::run() { 
    auto rate = std::chrono::milliseconds(16);
    auto t1 = std::chrono::steady_clock::now();
    float elapsed = 0.0f;

    while (this->should_continue() && elapsed < duration){
        ClientCommand cmd;
        while (gameloop->try_pop(cmd)) {
            this->execute(cmd);
        }
        update_phase();
        broadcast_phase();

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
        elapsed += std::chrono::duration<float>(rate).count();
    }
    this->end();
}

Phase::~Phase() {}