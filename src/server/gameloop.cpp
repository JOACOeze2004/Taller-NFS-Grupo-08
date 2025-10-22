#include "gameloop.h"

#include <cmath>
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
    ClientCommand cmd;
    while (cmd_queue.try_pop(cmd)) {
        CarState& car = cars[cmd.id];

        switch (cmd.cmd.cmd) {
            case SEND_ACCELERATE:
                car.velocity += 0.5;
                break;
            case SEND_ROTATE_LEFT:
                car.angle -= 0.1;
                break;
            case SEND_ROTATE_RIGHT:
                car.angle += 0.1;
                break;
            case SEND_BRAKE:
                car.velocity -= 0.3;
                break;
            default:
                break;
        }
    }
}

void Gameloop::update_positions() {
    for (auto& [id, car] : cars) {
        car.x += car.velocity * cos(car.angle);
        car.y += car.velocity * sin(car.angle);
    }
}

void Gameloop::broadcast(std::map<int, CarState>& _cars) const {
    monitor.broadcast(_cars);
}
