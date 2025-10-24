#include "gameloop.h"


#include "src/common/constants.h"

Gameloop::Gameloop(Queue<ClientCommand>& _cmd_queue, Monitor& _monitor):
        cmd_queue(_cmd_queue), monitor(_monitor){}

void Gameloop::run() {
    while (should_keep_running()) {
        process_commands();
        update_positions();
        broadcast(cars);
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // no es exacto todavia
    }
}

void Gameloop::process_commands() {
    ClientCommand cmd{};
    while (cmd_queue.try_pop(cmd)) {
        car& car = cars[cmd.id];

        std::cout << "[GL] cmd: " << int(cmd.cmd.cmd) << " vel: " << car.get_velocity() << " ang: " << car.get_angle() << std::endl;
        switch (cmd.cmd.cmd) {
            case SEND_ACCELERATE:
                car.accelerate();
                break;
            case SEND_ROTATE_LEFT:
                car.turn_left();
                break;
            case SEND_ROTATE_RIGHT:
                car.turn_right();
                break;
            case SEND_BRAKE:
                car.brake();
                break;
            default:
                break;
        }
    }
}

void Gameloop::add_car(const int client_id) {
    car car;
    cars[client_id] = car;
}

void Gameloop::update_positions() {
    for (auto& [id, car] : cars) {
        car.update_position();
    }
}

void Gameloop::broadcast(std::map<int, car>& _cars) const {
    monitor.broadcast(_cars);
}
