#include "workshop.h"

#include "../common/constants.h"
#include "gameloop.h"

Workshop::Workshop(Gameloop* _gameloop, float _duration)
    : Phase(_gameloop, _duration) {
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
    
    car_upgrades[SEND_ACCELERATION_DOWNGRADE] = [this](int& id) { gameloop->accelerate_downgrade(id); };
    car_upgrades[SEND_HANDLING_DOWNGRADE] = [this](int& id) { gameloop->handling_downgrade(id); };
    car_upgrades[SEND_NITRO_DOWNGRADE] = [this](int& id) { gameloop->nitro_downgrade(id); };
    car_upgrades[SEND_LIFE_DOWNGRADE] = [this](int& id) { gameloop->life_downgrade(id); };
    car_upgrades[SEND_BRAKE_DOWNGRADE] = [this](int& id) { gameloop->brake_downgrade(id); };
    car_upgrades[SEND_MASS_DOWNGRADE] = [this](int& id) { gameloop->mass_downgrade(id); };
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

bool Workshop::should_continue() {
    if (!gameloop->has_active_players()) {
        std::cout << "[WORKSHOP] No active players remaining, ending game" << std::endl;
        return false;
    }
    return gameloop->is_running();
}

void Workshop::update(int time_ms) { gameloop->broadcast_workshop(this->get_prices(),time_ms); } 

void Workshop::end() {
    if (!gameloop->has_active_players()) {
        std::cout << "[WORKSHOP] No players left, ending game" << std::endl;
        gameloop->stop();
        return;
    }

    gameloop->reset_race();

    std::cout << "[WORKSHOP] Starting next race (Race "
              << (gameloop->get_races_completed() + 1) << ")" << std::endl;
    gameloop->change_phase(std::make_unique<InGame>(gameloop, MAX_TIME_PER_RACE + RACE_START_TIME));
}

std::map<Upgrades, std::chrono::seconds> Workshop::get_prices() { return this->prices; }

State Workshop::get_current_phase_state() const { return IN_WORK_SHOP; }