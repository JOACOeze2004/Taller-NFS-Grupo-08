#ifndef TALLER_TP_WORKSHOP_H
#define TALLER_TP_WORKSHOP_H
#include <map>
#include <functional> 
#include "phase.h"
#include "../client_command.h"
#include "../../common/DTO.h"

class Workshop : public Phase {
public:
    explicit Workshop(Gameloop* _gameloop, float _duration);

    void execute(ClientCommand& command) override;
    bool should_continue() override;
    void end() override;
    void update(int time_ms) override;
    State get_current_phase_state() const override;
};


#endif  // TALLER_TP_WORKSHOP_H
