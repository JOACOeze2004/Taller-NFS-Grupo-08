#ifndef TALLER_TP_COUNTDOWN_H
#define TALLER_TP_COUNTDOWN_H
#include "phase.h"

class Countdown : public Phase {
public:
    Countdown(Gameloop* gameloop, float duration);

    void execute(ClientCommand& command) override;
    void update(int time_ms) override;
    bool should_continue() override;
    void end() override;

    State get_current_phase_state() const override;
};

#endif  // TALLER_TP_COUNTDOWN_H
