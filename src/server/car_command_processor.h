#ifndef TALLER_TP_CAR_COMMAND_PROCESSOR_H
#define TALLER_TP_CAR_COMMAND_PROCESSOR_H
#include <unordered_map>
#include <functional>
#include "car.h"
#include "race.h"
#include "src/common/constants.h"

class CarCommandProcessor {
private:
    std::unordered_map<int, std::function<void(Car&)>> car_actions;
    std::unordered_map<int, std::function<void(Race&, int)>> race_actions;

    void init_car_actions();
    void init_race_actions();
public:
    explicit CarCommandProcessor();

    const std::function<void(Car&)>* get_car_action(int cmd) const;
    const std::function<void(Race&, int)>* get_race_action(int cmd) const;

    ~CarCommandProcessor();

};


#endif  // TALLER_TP_CAR_COMMAND_PROCESSOR_H