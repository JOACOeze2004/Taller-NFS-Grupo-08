#include "client_handler.h"

ClientHandler::ClientHandler(Socket&& peer, Queue<ClientCommand>& commands, int _id):
        peer(std::move(peer)),
        protocol(this->peer),
        command_queue(commands),
        client_queue(CLIENT_QUEUE_LEN),
        id(_id),
        receiver(protocol, command_queue, this->id),
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

    receiver.start();
    sender.start();
}

void ClientHandler::send_state(CarDTO state) {
    client_queue.try_push(state);
}

void ClientHandler::kill() {
    stop();

    client_queue.close();
    receiver.stop();
    sender.stop();
    kill_threads();
}

void ClientHandler::kill_threads() {
    receiver.join();
    sender.join();
}

bool ClientHandler::is_dead() const { return !sender.is_alive() && !receiver.is_alive(); }

ClientHandler::~ClientHandler() {}
