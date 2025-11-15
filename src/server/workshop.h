#ifndef TALLER_TP_WORKSHOP_H
#define TALLER_TP_WORKSHOP_H
#include "client_command.h"
#include <map>
#include <functional> 
#include "phase.h"
#include "../common/DTO.h"


class Workshop : public Phase {
private:
    std::map<int, std::function<void(int&)>> car_upgrades;
    std::map<Upgrades, std::chrono::seconds> prices;

    void initialize_car_upgrades();
    void initialize_prices();
public:
    explicit Workshop(Gameloop* _gameloop, float _duration);
    std::map<Upgrades, std::chrono::seconds> get_prices();

    void execute(ClientCommand& command) override;
    bool should_continue() override;
    void end() override;
    void update_phase() override;
    void broadcast_phase(int time_ms) override;
    State get_current_phase_state() const override;
};


#endif  // TALLER_TP_WORKSHOP_H
