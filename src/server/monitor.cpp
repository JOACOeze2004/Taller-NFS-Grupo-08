#include "monitor.h"
#include "gameloop.h"

Monitor::Monitor() { game_id = 0; }

std::string Monitor::generate_game_id(){
    return std::to_string(game_id++);
}

cavoid Monitor::add_client(const int client_id, std::unique_ptr<ClientHandler> client) {
    std::unique_lock<std::mutex> lock(mutex);
    clients[client_id] = std::move(client);
}

void Monitor::reap() {
    for (size_t i = 1; i < clients.size();) {
        if (!clients.contains(i)) {
            continue;
        }
        auto& client = clients[i];
        if (!client->is_alive()) {
            client->kill();
            client->join();
            clients.erase(i);
        }else{
            i++;
        }
    }
}

void Monitor::clear_clients() {
    std::unique_lock<std::mutex> lock(mutex);
    for (size_t i = 1; i < clients.size(); i++) {
        if (!clients.contains(i)) {
            continue;
        }
        const auto& client = clients[i];
        client->kill();
        client->join();
    }
    clients.clear();
}

void Monitor::broadcast(std::map<int, CarState>& cars) {
    std::unique_lock<std::mutex> lock(mutex);
    for (auto& [id, car] : cars) {
        if (!clients.contains(id)) {
            continue;
        }
        clients[id]->send_state(car);
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
    players[username] = game_id;
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
    current_games.erase(_game_id);
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
