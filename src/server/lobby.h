#ifndef TALLER_TP_LOBBY_H
#define TALLER_TP_LOBBY_H
#include "phase.h"
#include "../common/constants.h"
#include <chrono>
#include <thread>


class Lobby : public Phase {
public:
    explicit Lobby(Gameloop* _gameloop, float _duration);
    void execute(ClientCommand& command) override;
    void update_phase() override;
    void broadcast_phase() override;
    bool should_continue() override;
    void end() override;
};


#endif  // TALLER_TP_LOBBY_H
