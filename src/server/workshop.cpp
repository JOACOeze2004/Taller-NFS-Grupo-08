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
}

void Workshop::initialize_prices() {
    prices[ACCELERATION_UPGRADE] = std::chrono::seconds(ACCELERATION_PRICE);
    prices[HANDLING_UPGRADE] = std::chrono::seconds(HANDLING_PRICE);
    prices[NITRO_UPGRADE] = std::chrono::seconds(NITRO_PRICE);
    prices[LIFE_UPGRADE] = std::chrono::seconds(LIFE_PRICE);
    prices[BRAKE_UPGRADE] = std::chrono::seconds(BRAKE_PRICE);
    prices[MASS_UPGRADE] = std::chrono::seconds(MASS_PRICE);
}

void Workshop::run() {
    auto rate = std::chrono::milliseconds(16);
    auto t1 = std::chrono::steady_clock::now();
    gameloop->broadcast_workshop();

    while (should_continue()) {

        execute();

        auto t2 = std::chrono::steady_clock::now();
        auto rest = rate - std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
        if (rest < std::chrono::milliseconds(0)) {
            auto behind = -rest;
            rest = rate - behind % rate;
            auto lost = behind + rest;
            t1 += lost;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(rest));
        t1 += rate;
    }
}

void Workshop::execute() {
    ClientCommand command{};
    while (gameloop->try_pop(command)) {
        gameloop->process_command(command);
        car_upgrades[command.cmd_struct.cmd];
    }
}

std::map<Upgrades, std::chrono::seconds> Workshop::get_prices() {
    return prices;
}


