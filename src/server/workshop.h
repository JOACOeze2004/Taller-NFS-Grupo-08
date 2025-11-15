#ifndef TALLER_TP_WORKSHOP_H
#define TALLER_TP_WORKSHOP_H
#include "client_command.h"
#include "phase.h"


class Workshop : public Phase {
public:
    explicit Workshop(Gameloop* _gameloop, float _duration);
    void run() override;
    void execute() override;
    std::map<Upgrades, std::chrono::seconds> get_prices();


    // metodos a implementar mas prolijamente
    explicit Workshop(Gameloop* _gameloop, float _duration);
    void execute(ClientCommand& command) override;
    bool should_continue() override;
    void end() override;
    void update_phase() override;
    void broadcast_phase() override;
};


#endif  // TALLER_TP_WORKSHOP_H
