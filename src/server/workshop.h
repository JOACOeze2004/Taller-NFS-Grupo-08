#ifndef TALLER_TP_WORKSHOP_H
#define TALLER_TP_WORKSHOP_H
#include "client_command.h"
#include "phase.h"


class Workshop : public Phase {
public:
    explicit Workshop(Gameloop* _gameloop, float _duration);
    void run() override;
    void execute(ClientCommand& command) override;
};


#endif  // TALLER_TP_WORKSHOP_H
