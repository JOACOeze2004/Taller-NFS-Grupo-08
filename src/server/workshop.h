#ifndef TALLER_TP_WORKSHOP_H
#define TALLER_TP_WORKSHOP_H
#include <chrono>
#include <functional>
#include <map>

#include "src/common/DTO.h"

#include "car.h"
#include "phase.h"


class Workshop : public Phase {
    std::map<uint8_t, std::function<void(int& id)>> car_upgrades;
    std::map<Upgrades, std::chrono::seconds> prices;

    void initialize_car_upgrades();
    void initialize_prices();

public:
    explicit Workshop(Gameloop* _gameloop, float _duration);
    void run() override;
    void execute() override;
    std::map<Upgrades, std::chrono::seconds> get_prices();
};


#endif  // TALLER_TP_WORKSHOP_H
