#ifndef TALLER_TP_PHASE_HANDLER_H
#define TALLER_TP_PHASE_HANDLER_H
#include "client_command.h"
class Gameloop;

class Phase {
protected:
    Gameloop *gameloop;
    float duration;
public:
    explicit Phase(Gameloop* _gameloop, float _duration);
    
    void run();

    virtual bool should_continue() = 0;
    virtual void execute(ClientCommand& command) = 0;
    virtual void end() = 0;
    virtual void update_phase() = 0;
    virtual void broadcast_phase() = 0;

    virtual ~Phase();
};


#endif  // TALLER_TP_PHASE_HANDLER_H
