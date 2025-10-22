#include "gameloop.h"
#include "monitor.h"

Gameloop::Gameloop(Queue<Command>& _cmd_queue, Monitor& _monitor)
    : monitor(_monitor), cmd_queue(_cmd_queue) {
}

void Gameloop::run() {
    while (true) {
        process_commands();
        update_game();
        broadcast();
    }
}

void Gameloop::process_commands() {
    return;
}

void Gameloop::update_game() {
    return;
}

void Gameloop::broadcast() {
    return;
}
