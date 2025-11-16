#include "client_handler.h"

#include "src/common/DTO.h"
#include "receiver.h"
#include "exceptions/GameFullException.h"
#include "exceptions/InvalidGameIDException.h"
#include "exceptions/InvalidPlayerNameException.h"
#include "exceptions/GameAlreadyStartedException.h"

ClientHandler::ClientHandler(Socket&& peer,Monitor& monitor, int _id):
        peer(std::move(peer)),
        protocol(this->peer),
        client_queue(CLIENT_QUEUE_LEN),
        monitor(monitor),
        game_id(""),
        id(_id),
        receiver(nullptr),
        sender(protocol, client_queue) {} 


void ClientHandler::receive_player_configuration() {
    protocol.receive_player_config(player_name, car_id, map_name);
    if (player_name.size() < MIN_NAME_LEN || player_name.size() > MAX_NAME_LEN) {
        throw InvalidPlayerNameException("must be between 3 and 16 characters");
    }
}

std::shared_ptr<Gameloop> ClientHandler::process_lobby_action() {
    uint8_t action;
    std::string game_id_to_join;
    std::shared_ptr<Gameloop> game;
    std::string g_id;

    protocol.receive_lobby_action(action, game_id_to_join);

    if (action == SEND_CREATE_GAME) {
        game = monitor.create_game(map_name,this->id, car_id,this->player_name);
        g_id = monitor.get_last_created_game_id();
        set_game_id(g_id);
        game->start();
    } 
    else if (action == SEND_JOIN_GAME) {
        game = monitor.join_game(player_name, game_id_to_join, this->id, car_id);       
        set_game_id(game_id_to_join);
    }
    else {
        throw std::runtime_error("Invalid lobby action");
    }
    return game;
}

void ClientHandler::send_initial_data() {
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

    protocol.send_ok();
    protocol.send_game_init_data(map_path, spawn_x, spawn_y);
}

void ClientHandler::run() {
    std::shared_ptr<Gameloop> game;
    try {
        receive_player_configuration();
        game = process_lobby_action();
        send_initial_data();
    } catch(const std::exception& e){
        protocol.send_error_message(e.what());
        return;
    } catch (...) {
        protocol.send_error_message("unexpected error");
        return;
    }

    receiver = std::make_shared<ClientReceiver>(protocol, game, id);
    receiver->start();
    sender.start();
    
}

void ClientHandler::send_state(Snapshot snapshot) {
    snapshot.id = id;
    client_queue.try_push(snapshot);
}

void ClientHandler::kill() {
    protocol.close();
    stop();
    if (!receiver){
        return;
    }    
    receiver->stop();
    sender.stop();
    client_queue.close();
    kill_threads();
}

void ClientHandler::kill_threads() {
    sender.join();
    receiver->join();
}

bool ClientHandler::is_dead() const { 
    if (!receiver) {
        return false;
    }
    return !receiver->is_alive(); 
}

void ClientHandler::set_game_id(const std::string& _game_id) { game_id = _game_id; }

const std::string& ClientHandler::get_game_id() const { return game_id; }

void ClientHandler::send_final_results(const FinalScoreList& results) {
    try {
        protocol.send_final_results(results);
        std::cout << "[SERVER CLIENT HANDLER] Final results sent successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[SERVER] Error sending final results to client " << id << ": " << e.what() << std::endl;
    }
}

ClientHandler::~ClientHandler() {}
