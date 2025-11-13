#ifndef TALLER_TP_GAMELOOP_H
#define TALLER_TP_GAMELOOP_H

#pragma once
#include <functional>
#include <map>

#include "../common/queue.h"
#include "../common/thread.h"

#include "car.h"
#include "client_command.h"
#include "in_game.h"
#include "lobby.h"
#include "monitor.h"
#include "race.h"
#include "workshop.h"
#include "world.h"

class Monitor;


class Gameloop: public Thread {

public:
    explicit Gameloop(Monitor& _monitor, const std::string& game_id, std::string map_name);
    void run() override;

    void add_car(const int client_id, const int car_id,  const std::string& player_name);
    void push_command(const ClientCommand& cmd);
    const std::string& get_game_id() const;
    bool can_join_to_game();
    bool is_username_taken(const int username_id) const;
    int get_time_remaining_ms() const;
    void process_command(ClientCommand& client_command);
    void broadcast_in_game();
    void broadcast_lobby();
    bool try_pop(ClientCommand& command);
    void update_positions();

    bool is_game_already_started() const;

private:
    Queue<ClientCommand> cmd_queue;
    std::string game_id;
    Monitor& monitor;
    std::map<int, Car> cars;
    std::map<uint8_t, std::function<void(Car& car)>> car_actions;
    World world;
    bool ready_to_start;
    Lobby lobby;
    InGame in_game;
    Workshop workshop;
    std::chrono::steady_clock::time_point start_time;
    Map current_map;
    ParserYaml parser;
    std::unordered_map<int, std::string> user_names;


    void initialize_car_actions();
    Snapshot initialize_DTO();
};


#endif  // TALLER_TP_GAMELOOP_H
