#include "monitor.h"
#include "gameloop.h"
#include "exceptions/GameFullException.h"
#include "exceptions/InvalidGameIDException.h"
#include "exceptions/InvalidPlayerNameException.h"
#include "exceptions/GameAlreadyStartedException.h"

Monitor::Monitor() { game_id = 0; }

std::string Monitor::generate_game_id(){
    return std::to_string(game_id++);
}

std::string Monitor::get_last_created_game_id() const {
    return std::to_string(game_id - 1);
}

std::string Monitor:: get_client_game_id(int client_id){
    auto it = clients.find(client_id);
    if (it != clients.end()) {
        return it->second->get_game_id();
    }
    return "";
}

void Monitor::add_client(const int client_id, std::unique_ptr<ClientHandler> client) {
    std::unique_lock<std::mutex> lock(mutex);
    clients[client_id] = std::move(client);
}

void Monitor::reap_clients() {
    std::vector<int> to_remove;
    for ( auto& [id, client] : clients) {
        if (client->is_dead()) {
            std::string g_id = client->get_game_id();
            client->kill();
            client->join();
            to_remove.push_back(id);
        }
    }

    for (auto id : to_remove) {
        clients.erase(id);
    }
}

void Monitor::reap_games() {
    std::vector<std::string> games_to_remove;
    
    for (auto& [game_id, gameloop] : current_games) {
        if (!gameloop->has_active_players() || !gameloop->is_alive()) {
            games_to_remove.push_back(game_id);
        }
    }

    for (const auto& game_id : games_to_remove) {
        auto game = current_games[game_id];
        game->stop();
        game->join();
        current_games.erase(game_id);
        clear_remaining_clients(game_id);
    }
}

void Monitor::reap() {
    std::unique_lock<std::mutex> lock(mutex);
    reap_clients();
    reap_games();
}

void Monitor::clear_clients() {
    std::unique_lock<std::mutex> lock(mutex);
    for ( auto& [id, client] : clients) {
        client->kill();
        client->join();
    }
    clients.clear();
}

void Monitor::broadcast(const Snapshot& snapshot, const std::string& gid, const int client_id) {
    std::unique_lock<std::mutex> lock(mutex);
    auto it = clients.find(client_id);
    if (it == clients.end()) {
        return;
    }
    
    auto& client = it->second;
    if (!client) {
        return;
    }
    
    if (client->get_game_id() == gid && !client->is_dead()) {
        client->send_state(snapshot);
    }
}

std::shared_ptr<Gameloop> Monitor::create_game(std::string map_name, const int client_id, const int car_id, const std::string& username) {
    std::unique_lock<std::mutex> lock(mutex);
    std::string id = generate_game_id();
    auto game_loop = std::make_shared<Gameloop>(*this, id, map_name, client_id);
    current_games[id] = game_loop;
    game_loop->add_car(client_id, car_id,username);    
    return game_loop;
}

std::shared_ptr<Gameloop> Monitor::join_game(const std::string& username, const std::string& _game_id, const int client_id, const int car_id) {
    std::unique_lock<std::mutex> lock(mutex);
    auto game_i = current_games.find(_game_id);
    if (game_i == current_games.end()){
        throw InvalidGameIDException();
    }
    auto game = game_i->second;
    // if (game->is_game_already_started()){
    //     throw GameAlreadyStartedException();
    // }    
    if (!game->can_join_to_game()){
        throw GameFullException();
    }
    if (game->is_username_taken(client_id)) {
        throw InvalidPlayerNameException("name already in use");
    }          
    players[username] = _game_id;
    game->add_car(client_id, car_id,username);
    return game;
}

std::vector<std::string> Monitor::get_active_games() {
    std::unique_lock<std::mutex> lock(mutex);
    std::vector<std::string> active_games;
    for(const auto& [id,game] : current_games){
        //if (game->is_running){
            active_games.push_back(id);
        //}
    }
    return active_games;
}

void Monitor::remove_player(const std::string& username){
    std::unique_lock<std::mutex> lock(mutex);
    players.erase(username);
}

void Monitor::remove_game(const std::string& _game_id){
    auto game_to_remove = current_games.find(_game_id);
    if (game_to_remove != current_games.end()) {
        auto game_loop = game_to_remove->second;
        game_loop->stop();
        game_loop->join();
        current_games.erase(_game_id);
    }  
    clear_remaining_clients(_game_id);  
}

void Monitor::clear_remaining_clients(const std::string& _game_id){
    for (auto i = players.begin(); i != players.end();) {
        if (i->second == _game_id){
            i = players.erase(i);
        }else{
            i++;
        }
    }  
}

void Monitor::kill_games() {
    for (auto& [id, gameloop] : current_games) {
        gameloop->stop();
        gameloop->join();
    }
    current_games.clear();
    players.clear();
}
void Monitor::broadcast_final_results(const FinalScoreList& results, const std::string& gid) {
    std::unique_lock<std::mutex> lock(mutex);
    for (auto& [id, client] : clients) {
        if (client->get_game_id() == gid && !client->is_dead()) {
            client->send_final_results(results);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}