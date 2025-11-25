#ifndef TALLER_TP_IN_GAME_H
#define TALLER_TP_IN_GAME_H
#include "phase.h"
#include "../../common/DTO.h"
#include "../race.h"
#include <chrono>
#include <thread>

class InGame : public Phase {
    bool race_started;
    
public:
    explicit InGame(Gameloop* _gameloop, float _duration);
    
    bool should_continue() override;
    void execute(ClientCommand& command) override;
    void end() override;
    void update(int time_ms) override;
    State get_current_phase_state() const override;
};


#endif  // TALLER_TP_IN_GAME_H
