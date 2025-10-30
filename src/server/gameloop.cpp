#include "gameloop.h"


#include "src/common/constants.h"

Gameloop::Gameloop(Queue<ClientCommand>& _cmd_queue, Monitor& _monitor):
        cmd_queue(_cmd_queue), monitor(_monitor) {
    initialize_car_actions();
}

void Gameloop::initialize_car_actions() {
    car_actions[SEND_ACCELERATE] = [](Car& car) { car.accelerate(); };
    car_actions[SEND_ROTATE_RIGHT] = [](Car& car) { car.turn_right(); };
    car_actions[SEND_ROTATE_LEFT] = [](Car& car) { car.turn_left(); };
    car_actions[SEND_BRAKE] = [](Car& car) { car.brake(); };

    car_actions[SEND_WIN_RACE_CHEAT] = [](Car& car) { car.activate_win_race(); };
    car_actions[SEND_RESTORE_LIFE_CHEAT] = [](Car& car) { car.restore_life(); };
    car_actions[SEND_INFINITE_LIFE_CHEAT] = [](Car& car) { car.activate_infinite_life(); };
    car_actions[SEND_LOSE_RACE_CHEAT] = [](Car& car) { car.activate_lose_race(); };
    car_actions[SEND_INFINITE_NITRO_CHEAT] = [](Car& car) { car.activate_infinite_nitro(); };
}

void Gameloop::run() {
    while (should_keep_running()) {
        process_commands();
        update_positions();
        broadcast(cars);
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // no es exacto todavia
    }
}

void Gameloop::process_commands() {
    ClientCommand client_command{};
    while (cmd_queue.try_pop(client_command)) {
        auto it = cars.find(client_command.id);
        if (it == cars.end()) {
            add_car(client_command.id);
        }
        it = cars.find(client_command.id);
        Car& car = it->second;
        auto action = car_actions.find(client_command.cmd_struct.cmd);
        if (action != car_actions.end()) {
            action->second(car);
        }
    }
}

void Gameloop::add_car(const int client_id) {
    cars.emplace(std::piecewise_construct,
    std::forward_as_tuple(client_id),
    std::forward_as_tuple(world.get_id()));
}

void Gameloop::update_positions() {
    world.update();
}

void Gameloop::broadcast(std::map<int, Car>& _cars) const {
    monitor.broadcast(_cars);
}
