#ifndef TALLER_TP_GAMELOOP_H
#define TALLER_TP_GAMELOOP_H

#pragma once
#include <functional>
#include <map>

#include "../common/queue.h"
#include "../common/thread.h"
#include "src/common/DTO.h"

#include "car.h"
#include "car_command_processor.h"
#include "client_command.h"
#include "phase/in_game.h"
#include "phase/lobby.h"
#include "monitor.h"
#include "npc.h"
#include "race.h"
#include "results_handler.h"
#include "phase/workshop.h"
#include "world.h"
#include "upgrade_handler.h"

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
    void broadcast_workshop(const int time_ms);
    void broadcast_countdown(const int time_ms);
    bool try_pop(ClientCommand& command);
    void update_positions();

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
    World world;
    int owner_id;
    bool ready_to_start;
    Map current_map;
    ParserYaml parser;
    std::unordered_map<int, std::string> user_names;
    Race race;
    std::unique_ptr<Phase> current_phase;
    int races_completed{};
    bool game_started;
    CarCommandProcessor command_processor;
    ResultsHandler results;
    std::vector<GraphNode> corners;
    std::vector<NPC> npcs;
    UpgradeHandler upgrader; 
    
    Snapshot initialize_DTO();
    std::unordered_map<int, CarDTO> build_cars_dto(std::function<StateRunning(int)> car_state);
    Snapshot build_base_snapshot (const std::unordered_map<int, CarDTO>& cars_DTO, State state, int time_ms, int player_id);
    void generate_npcs();
    
    bool handle_disconnect(const int command_id, const int id);
    bool handle_upgrade(const int command, Car& car, const int player_id);

    bool handle_car_action(const int command, Car& car);
    bool handle_race_action(const int command, int id);
    bool handle_skip_to_last_race_cheat(const int command);
    bool handle_force_start(const int command);
    Car* find_car(const int id);




    template<typename CustomizedSnapshot>
    void common_broadcast(State state, int time_ms, std::function<StateRunning(int)> state_fn, 
        CustomizedSnapshot custom_snapshot) {
            auto cars_DTO = build_cars_dto(state_fn);
            for (auto& [id, car] : cars) {
                Snapshot dto = build_base_snapshot(cars_DTO, state, time_ms, id);
                custom_snapshot(dto, id);
                monitor.broadcast(dto, std::string(game_id) , id);
            }
        }
    };


#endif  // TALLER_TP_GAMELOOP_H
