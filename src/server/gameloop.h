#ifndef TALLER_TP_GAMELOOP_H
#define TALLER_TP_GAMELOOP_H

#pragma once
#include <functional>
#include <map>

#include "../common/queue.h"
#include "../common/thread.h"

#include "car.h"
#include "client_command.h"
#include "monitor.h"

class Monitor;


class Gameloop: public Thread {

public:
    explicit Gameloop(Queue<ClientCommand>& _cmd_queue, Monitor& _monitor);
    void run() override;

    void add_car(const int client_id);
private:
    Queue<ClientCommand>& cmd_queue;
    Monitor& monitor;
    std::map<int, Car> cars;
    std::map<uint8_t, std::function<void(Car& car)>> car_actions;
    void initialize_car_actions();
    // Parser parser;
    // Workshop workshop; para entre carreras mejorar el auto
    // GameMap game_map; quiza que guarde todos los mapas y circuitos posibles (lo recibe por parametro)



    void process_commands();
    void update_positions();
    void broadcast(std::map<int, Car>& _cars) const;
};


#endif  // TALLER_TP_GAMELOOP_H
