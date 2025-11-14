#ifndef TALLER_TP_LOBBY_H
#define TALLER_TP_LOBBY_H
#include "phase.h"
#include "../common/constants.h"
#include <chrono>
#include <thread>


class Lobby : public Phase {
    int owner_id;
public:
    explicit Lobby(Gameloop* _gameloop, float _duration, const int owner_id);
    void run() override;
    void execute() override;
};


#endif  // TALLER_TP_LOBBY_H
