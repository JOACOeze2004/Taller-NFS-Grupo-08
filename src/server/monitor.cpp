#include "monitor.h"

Monitor::Monitor() { game_id = 0; }

std::string Monitor::generate_game_id(){
    return std::to_string(game_id++);
}

void Monitor::add_client(std::unique_ptr<ClientHandler> client) {
    std::unique_lock<std::mutex> lock(mutex);
    clients.push_back(std::move(client));
}

void Monitor::reap() {
    for (size_t i = 0; i < clients.size();) {
        auto& client = clients[i];
        if (!client->is_alive()) {
            client->kill();
            client->join();
            clients.erase(clients.begin() + i);
        }else{
            i++;
        }
    }
}

void Monitor::clear_clients() {
    std::unique_lock<std::mutex> lock(mutex);
    for (auto& client : clients) {
        client->kill();
        client->join();
    }
    clients.clear();
}

void Monitor::broadcast() {
    std::unique_lock<std::mutex> lock(mutex);
    return;
}

std::shared_ptr<Gameloop> Monitor::create_game(Queue<Command>& cmd_queue) {
    std::unique_lock<std::mutex> lock(mutex);
    std::string id = generate_game_id();
    auto game_loop = std::make_shared<Gameloop>(cmd_queue,*this);
    current_games[id] = game_loop;
    return game_loop;
}

std::shared_ptr<Gameloop> Monitor::join_game(const std::string& username, const std::string& game_id) {
    std::unique_lock<std::mutex> lock(mutex);
    auto game = current_games.find(game_id);
    if (game == current_games.end()){
        return nullptr;
    }
    //quizas handelear si el game esta muerto, tirar una excepcion de q la palmo la partida.
    players[username] = game_id;
    return  game->second;
}

Gameloop& Monitor::get_game(const std::string& game_id) {
    std::unique_lock<std::mutex> lock(mutex);
    return *(current_games.at(game_id));
}

void Monitor::remove_player(const std::string& username){
    std::unique_lock<std::mutex> lock(mutex);
    players.erase(username);
}

void Monitor::remove_player(const std::string& game_id){
    std::unique_lock<std::mutex> lock(mutex);
    current_games.erase(game_id);
    for (auto i = players.begin(); i != players.end();) {
        if (i->second == game_id){
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
