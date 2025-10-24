#include "monitor.h"
#include "gameloop.h"

Monitor::Monitor() { game_id = 0; }

std::string Monitor::generate_game_id(){
    return std::to_string(game_id++);
}

std::string Monitor::get_last_created_game_id() const {
    return std::to_string(game_id - 1);
}

void Monitor::add_client(const int client_id, std::unique_ptr<ClientHandler> client) {
    std::unique_lock<std::mutex> lock(mutex);
    clients[client_id] = std::move(client);
    auto i = current_games.find(std::to_string(game_id));
    if (i != current_games.end()){
        i->second->add_car(client_id);
    }
}

void Monitor::reap() {
    std::unique_lock lock(mutex);
    std::vector<int> to_remove;
    for ( auto& [id, client] : clients) {
        if (client->is_dead()) {
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
    std::unique_lock<std::mutex> lock(mutex);
    for ( auto& [id, client] : clients) {
        if (client->is_dead()) {
            client->kill();
            client->join();
        }
    }
    clients.clear();
}

void Monitor::broadcast(std::map<int, car>& cars) {
    std::unique_lock<std::mutex> lock(mutex);
    for (auto& [id, car] : cars) {
        auto i = clients.find(id);
        if (i == clients.end()) {
            continue;
        }
        i->second->send_state(car.get_state());
    }
}

std::shared_ptr<Gameloop> Monitor::create_game(Queue<ClientCommand>& cmd_queue) {
    std::unique_lock<std::mutex> lock(mutex);
    std::string id = generate_game_id();
    auto game_loop = std::make_shared<Gameloop>(cmd_queue,*this);
    current_games[id] = game_loop;
    return game_loop;
}

std::shared_ptr<Gameloop> Monitor::join_game(const std::string& username, const std::string& _game_id) {
    std::unique_lock<std::mutex> lock(mutex);
    auto game = current_games.find(_game_id);
    if (game == current_games.end()){
        return nullptr;
    }
    //quizas handelear si el game esta muerto, tirar una excepcion de q la palmo la partida.
    players[username] = _game_id;
    return  game->second;
}

Gameloop& Monitor::get_game(const std::string& _game_id) {
    std::unique_lock<std::mutex> lock(mutex);
    return *(current_games.at(_game_id));
}

void Monitor::remove_player(const std::string& username){
    std::unique_lock<std::mutex> lock(mutex);
    players.erase(username);
}

void Monitor::remove_game(const std::string& _game_id){
    std::unique_lock<std::mutex> lock(mutex);
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
    std::unique_lock<std::mutex> lock(mutex);
    for (auto& [id, gameloop] : current_games) {
        gameloop->stop();
        gameloop->join();
    }
    current_games.clear();
    players.clear();
}
