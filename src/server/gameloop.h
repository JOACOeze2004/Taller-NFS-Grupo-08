#ifndef TALLER_TP_GAMELOOP_H
#define TALLER_TP_GAMELOOP_H

#pragma once
#include "../common/queue.h"
#include "../common/thread.h"

#include "car_state.h"
#include "client_command.h"
#include "monitor.h"
#include <map>

class Monitor;


class Gameloop: public Thread {

public:
    explicit Gameloop(Queue<ClientCommand>& _cmd_queue, Monitor& _monitor);
    void run() override;

private:
    Queue<ClientCommand>& cmd_queue;
    Monitor& monitor;
    std::map<int, CarState> cars;
    // Parser parser;
    // Workshop workshop; para entre carreras mejorar el auto
    // GameMap game_map; quiza que guarde todos los mapas y circuitos posibles (lo recibe por parametro)



    void process_commands();
    void update_positions();
    void broadcast(std::map<int, CarState>& _cars) const;
};


#endif  // TALLER_TP_GAMELOOP_H
