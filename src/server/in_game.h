#ifndef TALLER_TP_IN_GAME_H
#define TALLER_TP_IN_GAME_H
#include "phase.h"
#include "../common/DTO.h"
#include "race.h"
#include <chrono>
#include <thread>


class InGame : public Phase {
    std::chrono::steady_clock::time_point start_time;
    bool race_started;
public:
    explicit InGame(Gameloop* _gameloop, float _duration);
    
    bool should_continue() override;
    void execute(ClientCommand& command) override;
    void end() override;
    void update_phase() override;
    void broadcast_phase() override;
    int get_time_remaining_ms() const;
};


#endif  // TALLER_TP_IN_GAME_H
