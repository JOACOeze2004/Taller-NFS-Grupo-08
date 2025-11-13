#ifndef TALLER_TP_PHASE_HANDLER_H
#define TALLER_TP_PHASE_HANDLER_H
#include "client_command.h"
class Gameloop;

class Phase {
protected:
    Gameloop *gameloop;
    float duration;
    bool cont;
public:
    explicit Phase(Gameloop* _gameloop, float _duration);
    virtual void run() = 0;
    virtual bool should_continue();
    virtual void execute(ClientCommand& command) = 0;
    virtual ~Phase();
};


#endif  // TALLER_TP_PHASE_HANDLER_H
