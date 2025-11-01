#ifndef TALLER_TP_GAMELOOP_H
#define TALLER_TP_GAMELOOP_H

#pragma once
#include <functional>
#include <map>
#include "world.h"

#include "../common/queue.h"
#include "../common/thread.h"

#include "car.h"
#include "client_command.h"
#include "monitor.h"

class Monitor;


class Gameloop: public Thread {

public:
    explicit Gameloop(Monitor& _monitor, const std::string& game_id);
    void run() override;

    void add_car(const int client_id);
    void push_command(const ClientCommand& cmd);
    const std::string& get_game_id() const;
private:
    Queue<ClientCommand> cmd_queue;
    std::string game_id;
    Monitor& monitor;
    std::map<int, Car> cars;
    std::map<uint8_t, std::function<void(Car& car)>> car_actions;
    World world;
    void initialize_car_actions();
    // Parser parser;
    // Workshop workshop; para entre carreras mejorar el auto
    // GameMap game_map; quiza que guarde todos los mapas y circuitos posibles (lo recibe por parametro)



    void process_commands();
    void update_positions();
    void broadcast() const;
};


#endif  // TALLER_TP_GAMELOOP_H
