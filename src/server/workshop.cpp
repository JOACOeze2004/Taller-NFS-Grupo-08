#include "workshop.h"

#include "../common/constants.h"
#include "gameloop.h"

Workshop::Workshop(Gameloop* _gameloop, float _duration)
    : Phase(_gameloop, _duration), phase_started(false) {
    initialize_car_upgrades();
    initialize_prices();
}
void Workshop::initialize_car_upgrades() {
    car_upgrades[SEND_ACCELERATION_UPGRADE] = [this](int& id) { gameloop->accelerate_upgrade(id); };
    car_upgrades[SEND_HANDLING_UPGRADE] = [this](int& id) { gameloop->handling_upgrade(id); };
    car_upgrades[SEND_NITRO_UPGRADE] = [this](int& id) { gameloop->nitro_upgrade(id); };
    car_upgrades[SEND_LIFE_UPGRADE] = [this](int& id) { gameloop->life_upgrade(id); };
    car_upgrades[SEND_BRAKE_UPGRADE] = [this](int& id) { gameloop->brake_upgrade(id); };
    car_upgrades[SEND_MASS_UPGRADE] = [this](int& id) { gameloop->mass_upgrade(id); };
}

void Workshop::initialize_prices() {
    prices[ACCELERATION_UPGRADE] = std::chrono::seconds(ACCELERATION_PRICE);
    prices[HANDLING_UPGRADE] = std::chrono::seconds(HANDLING_PRICE);
    prices[NITRO_UPGRADE] = std::chrono::seconds(NITRO_PRICE);
    prices[LIFE_UPGRADE] = std::chrono::seconds(LIFE_PRICE);
    prices[BRAKE_UPGRADE] = std::chrono::seconds(BRAKE_PRICE);
    prices[MASS_UPGRADE] = std::chrono::seconds(MASS_PRICE);
}

void Workshop::execute(ClientCommand& command) {
    gameloop->process_command(command);
    auto it = car_upgrades.find(command.cmd_struct.cmd);
    if (it != car_upgrades.end()) {
        it->second(command.id);
    }
}

bool Workshop::should_continue() { return gameloop->is_running() && get_time_remaining_ms() > 0; }

void Workshop::update_phase() {
    if (!phase_started){
        phase_started = true;
        start_time = std::chrono::steady_clock::now();
    }    
} 

void Workshop::end() { gameloop->change_phase(std::make_unique<InGame>(gameloop, MAX_TIME_PER_RACE)); }

void Workshop::broadcast_phase() { gameloop->broadcast_workshop(this->get_prices(),this->get_time_remaining_ms());}


std::map<Upgrades, std::chrono::seconds> Workshop::get_prices() { return this->prices; }

int Workshop::get_time_remaining_ms() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
    return std::max(0, MAX_TIME_PER_WORKSHOP - static_cast<int>(elapsed));
}


