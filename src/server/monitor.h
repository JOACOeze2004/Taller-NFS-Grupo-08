#ifndef TALLER_TP_MONITOR_H
#define TALLER_TP_MONITOR_H
#pragma once
#include <map>
#include <memory>
#include <unordered_map>

#include "src/common/DTO.h"
#include "../common/constants.h"

#include "car.h"
#include "client_command.h"
#include "client_handler.h"

class ClientHandler;
class Gameloop;

class Monitor {
private:
    std::mutex mutex;
    int game_id;    
    std::unordered_map<std::string, std::string> players;
    std::unordered_map<std::string, std::shared_ptr<Gameloop>> current_games;

    std::map<int, std::unique_ptr<ClientHandler>> clients;

    std::string generate_game_id();
    void clear_remaining_clients(const std::string& _game_id);
    std::string get_client_game_id(int client_id);
    void reap_clients();
    void reap_games();

public:
    Monitor();
    void add_client(const int client_id, std::unique_ptr<ClientHandler> client);
    void broadcast(const Snapshot& snapshot,const std::string& gid, int client_id);
    void broadcast_final_results(const FinalScoreList& results, const std::string& gid);
    void clear_clients();
    void reap();

    std::shared_ptr<Gameloop> create_game(std::string map_name, const int client_id, const int car_id,const std::string& username);
    std::shared_ptr<Gameloop> join_game(const std::string& user_name, const std::string& game_id, const int client_id, const int car_id);
    std::vector<std::string> get_active_games();
    std::string get_last_created_game_id() const;

    void kill_games();

};


#endif  // TALLER_TP_MONITOR_H
