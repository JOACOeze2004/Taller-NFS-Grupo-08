#ifndef TALLER_TP_LOBBY_H
#define TALLER_TP_LOBBY_H
#include "phase.h"
#include "../../common/constants.h"
#include <chrono>
#include <thread>


class Lobby : public Phase {
public:
    explicit Lobby(Gameloop* _gameloop, float _duration);
    void execute(ClientCommand& command) override;
    void update(int time_ms) override;
    bool should_continue() override;
    void end() override;
    State get_current_phase_state() const override;
};


#endif  // TALLER_TP_LOBBY_H
