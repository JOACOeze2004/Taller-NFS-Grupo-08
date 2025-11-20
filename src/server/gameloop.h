#ifndef TALLER_TP_GAMELOOP_H
#define TALLER_TP_GAMELOOP_H

#pragma once
#include <functional>
#include <map>

#include "../common/queue.h"
#include "../common/thread.h"
#include "src/common/DTO.h"

#include "car.h"
#include "client_command.h"
#include "in_game.h"
#include "lobby.h"
#include "monitor.h"
#include "race.h"
#include "workshop.h"
#include "world.h"
#include "car_command_processor.h"

class Monitor;


class Gameloop: public Thread {

public:
    explicit Gameloop(Monitor& _monitor, const std::string& game_id, std::string map_name, const int client_id);
    void run() override;

    void add_car(const int client_id, const int car_id,  const std::string& player_name);
    void push_command(const ClientCommand& cmd);
    const std::string& get_game_id() const;
    bool can_join_to_game();
    bool is_username_taken(const int username_id) const;
    void process_command(ClientCommand& client_command);
    void broadcast_in_game(const int time_ms);
    void broadcast_final_results(const FinalScoreList& results);
    void broadcast_lobby(const int time_ms);
    void broadcast_workshop(std::map<Upgrades, std::chrono::seconds> prices, const int time_ms);
    bool try_pop(ClientCommand& command);
    void update_positions();
    void accelerate_upgrade(int& id);
    void handling_upgrade(int& id);
    void nitro_upgrade(int& id);
    void life_upgrade(int& id);
    void brake_upgrade(int& id);
    void mass_upgrade(int& id);
    
    void accelerate_downgrade(int& id);
    void handling_downgrade(int& id);
    void nitro_downgrade(int& id);
    void life_downgrade(int& id);
    void brake_downgrade(int& id);
    void mass_downgrade(int& id);

    bool is_game_already_started() const;
    void handle_lobby_command(const ClientCommand& cmd);
    void change_phase(std::unique_ptr<Phase> new_phase);
    void update_race_state();

    bool is_running() const;
    bool did_all_finish();
    void start_race();
    std::string select_track_yaml(const std::string& map_name);

    FinalScoreList calculate_final_results();
    void increment_race_counter();
    int get_races_completed() const;
    void reset_race();
    bool has_active_players() const;

private:
    Queue<ClientCommand> cmd_queue;
    std::string game_id;
    Monitor& monitor;
    std::map<int, Car> cars;
    std::map<uint8_t, std::function<void(Car& car)>> car_actions;
    std::map<uint8_t, std::function<void(Race& car, int& id)>> race_actions;
    World world;
    int owner_id;
    bool ready_to_start;
    Map current_map;
    ParserYaml parser;
    std::unordered_map<int, std::string> user_names;
    Race race;
    std::unique_ptr<Phase> current_phase;
    std::map<int, long long> player_total_times;
    int races_completed{};
    std::vector<CarRacingInfo> finished;
    std::vector<CarRacingInfo> deads;
    bool game_started;
    CarCommandProcessor command_processor;
    
    Snapshot initialize_DTO();
};


#endif  // TALLER_TP_GAMELOOP_H
