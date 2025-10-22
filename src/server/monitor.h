#ifndef TALLER_TP_MONITOR_H
#define TALLER_TP_MONITOR_H
#pragma once
#include <unordered_map>
#include <memory>
#include "client_handler.h"
#include "../common/command.h"

class Gameloop;

class Monitor {
private:
    std::mutex mutex;
    int game_id;    
    std::unordered_map<std::string, std::string> players; //{Nombre de usuario, id partida}
    std::unordered_map<std::string, std::shared_ptr<Gameloop>> current_games; // {id partida, ptr a esa partida}

    std::vector<std::unique_ptr<ClientHandler>> clients; //Este en el futuro, vuela.

    std::string generate_game_id();

public:
    Monitor();
    void add_client(std::unique_ptr<ClientHandler> client);
    void broadcast();
    void clear_clients();
    void reap();

    std::shared_ptr<Gameloop> create_game(Queue<Command>& cmd_queue);
    std::shared_ptr<Gameloop> join_game(const std::string& user_name, const std::string& game_id);
    Gameloop& get_game(const std::string& game_id);
    void remove_player(const std::string& username);
    void remove_game(const std::string& user_id);

    void kill_games();

};


#endif  // TALLER_TP_MONITOR_H
