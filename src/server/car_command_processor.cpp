#include "car_command_processor.h"

CarCommandProcessor::CarCommandProcessor() {
    init_car_actions();
    init_race_actions();
}

void CarCommandProcessor::init_car_actions(){
    car_actions[SEND_ACCELERATE] = [](Car& car) { car.accelerate(); };
    car_actions[SEND_ROTATE_RIGHT] = [](Car& car) { car.turn_right(); };
    car_actions[SEND_ROTATE_LEFT] = [](Car& car) { car.turn_left(); };
    car_actions[SEND_BRAKE] = [](Car& car) { car.brake(); };
    car_actions[SEND_USE_NITRO] = [](Car& car) { car.toggle_nitro_status(); };

    car_actions[SEND_RESTORE_LIFE_CHEAT] = [](Car& car) { car.restore_life(); };
    car_actions[SEND_INFINITE_LIFE_CHEAT] = [](Car& car) { car.activate_infinite_life(); };
    car_actions[SEND_LOSE_RACE_CHEAT] = [](Car& car) { car.activate_lose_race(); };
    car_actions[SEND_INFINITE_NITRO_CHEAT] = [](Car& car) { car.activate_infinite_nitro(); };
}

void CarCommandProcessor::init_race_actions(){
    race_actions[SEND_WIN_RACE_CHEAT] = [](Race& race, int id) { race.activate_win(id); };
    race_actions[SEND_NEXT_CHECKPOINT] = [](Race& race, int id) { race.pass_to_next_checkpoint(id); };
}

const std::function<void(Car&)>* CarCommandProcessor::get_car_action(int cmd) const {
    auto it = car_actions.find(cmd);
    if (it != car_actions.end()){
        return &it->second;
    } 
    return nullptr;
}

const std::function<void(Race&, int)>* CarCommandProcessor::get_race_action(int cmd) const {
    auto it = race_actions.find(cmd);
    if (it != race_actions.end()){
        return &it->second;
    } 
    return nullptr;
}

CarCommandProcessor::~CarCommandProcessor(){};