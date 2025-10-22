#ifndef TALLER_TP_GAMELOOP_H
#define TALLER_TP_GAMELOOP_H

#pragma once
#include "../common/queue.h"
#include "../common/thread.h"

#include "client_command.h"

class Monitor;


class Gameloop: public Thread {

public:
    explicit Gameloop(Queue<Command>& _cmd_queue, Monitor& _monitor);
    void run() override;

private:
    Monitor& monitor;
    Queue<Command>& cmd_queue;
    // Parser parser;
    // Workshop workshop; para entre carreras mejorar el auto
    // GameMap game_map; quiza que guarde todos los mapas y circuitos posibles (lo recibe por parametro)



    void process_commands();
    void update_game();
    void broadcast();
};


#endif  // TALLER_TP_GAMELOOP_H
