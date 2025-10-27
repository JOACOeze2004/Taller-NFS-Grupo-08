#include "gameloop.h"


#include "src/common/constants.h"

Gameloop::Gameloop(Queue<ClientCommand>& _cmd_queue, Monitor& _monitor):
        cmd_queue(_cmd_queue), monitor(_monitor) { initialize_car_actions(); }

void Gameloop::initialize_car_actions() {
    car_actions[SEND_ACCELERATE] = [](Car& car) { car.accelerate(); };
    car_actions[SEND_ROTATE_RIGHT] = [](Car& car) { car.turn_right(); };
    car_actions[SEND_ROTATE_LEFT] = [](Car& car) { car.turn_left(); };
    car_actions[SEND_BRAKE] = [](Car& car) { car.brake(); };
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
        Car& car = cars[client_command.id];
        auto action = car_actions.find(client_command.cmd_struct.cmd);
        if (action != car_actions.end()) {
            action->second(car);
        }
    }
}

void Gameloop::add_car(const int client_id) {
    Car car;
    cars[client_id] = car;
}

void Gameloop::update_positions() {
    for (auto& [id, car] : cars) {
        car.update_position();
    }
}

void Gameloop::broadcast(std::map<int, Car>& _cars) const {
    monitor.broadcast(_cars);
}
