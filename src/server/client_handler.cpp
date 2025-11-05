#include "client_handler.h"

#include "src/common/DTO.h"
#include "receiver.h"

ClientHandler::ClientHandler(Socket&& peer,Monitor& monitor, int _id):
        peer(std::move(peer)),
        protocol(this->peer),
        client_queue(CLIENT_QUEUE_LEN),
        monitor(monitor),
        game_id(""),
        id(_id),
        sender(protocol, client_queue) {} 

void ClientHandler::run() {
    try {
        protocol.receive_player_config(player_name, car_id, map_name);
        std::cout << "[SERVER " << id << "] Received config:" << std::endl;
        std::cout << "  Player: " << player_name << std::endl;
        std::cout << "  Car Number: " << static_cast<int>(car_id) << std::endl;
        std::cout << "  Map: " << map_name << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[SERVER " << id << "] Error receiving config: " << e.what() << std::endl;
        return;
    }

    uint8_t action;
    std::string game_id_to_join;
    protocol.receive_lobby_action(action, game_id_to_join);    
    std::shared_ptr<Gameloop> game;
    std::string g_id;
    
    if (action == SEND_CREATE_GAME) {
        game = monitor.create_game(map_name,this->id);
        g_id = monitor.get_last_created_game_id();
        set_game_id(g_id);
        game->start();

    } else if (action == SEND_JOIN_GAME) {
        game = monitor.join_game(player_name, game_id_to_join, this->id);
        if (!game) {
            std::cerr << "[SERVER] Game not found or is full " << std::endl;
            return;
        }        
        g_id = game_id_to_join;
        set_game_id(game_id_to_join);
        
    } else {
        std::cerr << "[SERVER " << id << "] Invalid lobby action: " << static_cast<int>(action) << std::endl;
        return;
    }

    std::string map_path;
    if (map_name == "Liberty City") {
        map_path = "../assets/need-for-speed/cities/Game Boy _ GBC - Grand Theft Auto - Backgrounds - Liberty City.png";
    } else if (map_name == "San Andreas") {
        map_path = "../assets/need-for-speed/cities/Game Boy _ GBC - Grand Theft Auto - Backgrounds - San Andreas.png";
    } else if (map_name == "Vice City") {
        map_path = "../assets/need-for-speed/cities/Game Boy _ GBC - Grand Theft Auto - Backgrounds - Vice City.png";
    }

    float spawn_x = 200.0f ;
    float spawn_y = 200.0f;

    try {
        protocol.send_game_init_data(map_path, spawn_x, spawn_y);
        std::cout << "[SERVER " << id << "] Sent game init data" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[SERVER " << id << "] Error sending init data: " << e.what() << std::endl;
        return;
    }

    receiver = std::make_shared<ClientReceiver>(protocol, game, id);
    receiver->start();
    sender.start();
}

void ClientHandler::send_state(DTO dto) {
    dto.id = id;
    client_queue.try_push(dto);
}

void ClientHandler::kill() {
    stop();

    client_queue.close();
    receiver->stop();
    sender.stop();
    kill_threads();
}

void ClientHandler::kill_threads() {
    receiver->join();
    sender.join();
}

bool ClientHandler::is_dead() const { return !sender.is_alive() && !receiver->is_alive(); }

void ClientHandler::set_game_id(const std::string& _game_id) { game_id = _game_id; }

const std::string& ClientHandler::get_game_id() const { return game_id; }

ClientHandler::~ClientHandler() {}
