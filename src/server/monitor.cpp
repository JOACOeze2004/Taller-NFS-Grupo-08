#include "monitor.h"
#include "gameloop.h"

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

void Monitor::reap() {
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

void Monitor::clear_clients() {
    for ( auto& [id, client] : clients) {
        std::string g_id = client->get_game_id();
        client->kill();
        client->join();
    }
    clients.clear();
}

void Monitor::broadcast(Snapshot& snapshot, const std::string& gid) {
    std::unique_lock<std::mutex> lock(mutex);
    for (auto& [id, client] : clients) {
        if (client->get_game_id() == gid && !client->is_dead()) {
            client->send_state(snapshot);
        }
    }
}

std::shared_ptr<Gameloop> Monitor::create_game(std::string map_name, const int client_id, const int car_id) {
    std::unique_lock<std::mutex> lock(mutex);
    std::string id = generate_game_id();
    auto game_loop = std::make_shared<Gameloop>(*this, id, map_name);
    current_games[id] = game_loop;
    game_loop->add_car(client_id, car_id);
    return game_loop;
}

std::shared_ptr<Gameloop> Monitor::join_game(const std::string& username, const std::string& _game_id, const int client_id, const int car_id) {
    std::unique_lock<std::mutex> lock(mutex);
    auto game_i = current_games.find(_game_id);
    if (game_i == current_games.end()){
        return nullptr;
    }
    //quizas handelear si el game esta muerto, tirar una excepcion de q la palmo la partida.
    auto game = game_i->second;
    if (!game->can_join_to_game()){
        return nullptr;
    }    
    players[username] = _game_id;
    game->add_car(client_id, car_id);
    return game;
}

std::vector<std::string> Monitor::get_active_games() {
    std::unique_lock<std::mutex> lock(mutex);
    std::vector<std::string> active_games;
    for(const auto& [id,game] : current_games){
        //valdiar q este vivo el juego obvio.
        active_games.push_back(id);
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
