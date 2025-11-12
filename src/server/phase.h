#ifndef TALLER_TP_PHASE_HANDLER_H
#define TALLER_TP_PHASE_HANDLER_H
#include "gameloop.h"


class Phase {
protected:
    Gameloop *gameloop;
    float& duration;
    bool cont;
public:
    explicit Phase(Gameloop* _gameloop, float _duration);
    virtual void run();
    virtual bool should_continue();
    virtual void execute(ClientCommand& command);
    virtual ~Phase();
};


#endif  // TALLER_TP_PHASE_HANDLER_H
