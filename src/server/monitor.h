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
    std::unordered_map<std::string, std::string> players; //{Nombre de usuario, id partida}
    std::unordered_map<std::string, std::shared_ptr<Gameloop>> current_games; // {id partida, ptr a esa partida}
    std::unordered_map<std::string, int> game_player_count; // {game_id, cantidad_jugadores} 

    std::map<int, std::unique_ptr<ClientHandler>> clients; //Este en el futuro, vuela.

    std::string generate_game_id();
    void clear_remaining_clients(const std::string& _game_id);
    std::string get_client_game_id(int client_id);

public:
    Monitor();
    void add_client(const int client_id, std::unique_ptr<ClientHandler> client);
    void broadcast(DTO& dto,const std::string& gid);
    void clear_clients();
    void reap();

    std::shared_ptr<Gameloop> create_game(std::string map_name);
    std::shared_ptr<Gameloop> join_game(const std::string& user_name, const std::string& game_id);
    Gameloop& get_game(const std::string& game_id);
    void remove_player(const std::string& username);
    void remove_game(const std::string& user_id);
    std::string get_last_created_game_id() const;

    void kill_games();

};


#endif  // TALLER_TP_MONITOR_H
