#ifndef TALLER_TP_LOBBY_H
#define TALLER_TP_LOBBY_H
#include "phase.h"


class Lobby : public Phase {

public:
    explicit Lobby(Gameloop* _gameloop, float _duration);
    void run() override;
    void execute(ClientCommand& command) override;
};


#endif  // TALLER_TP_LOBBY_H
