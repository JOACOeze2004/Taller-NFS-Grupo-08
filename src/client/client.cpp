#include "client.h"
#include "client_protocol.h"
#include "client_receiver.h"

#include <iostream>
#include "client_handler.h"
#include "../common/queue.h"
#include "client_car.h"
#include "ClientQuitException.h"
#include "graphic_client.h"

Client::Client(const std::string& host, const std::string& port)
    : host(host), port(port), client_socket(host.c_str(), port.c_str()), protocol(client_socket), final_results_received(false)  {}

void Client::send_config(const PlayerConfig& config,uint8_t lobby_action, const std::string& game_id) {
    std::cout << "[CLIENT] Connected to " << host << ":" << port << std::endl;

    try {
        protocol.send_player_config(config.playerName, config.carId, config.mapName);
        protocol.send_lobby_action(lobby_action, game_id);

        uint8_t response = protocol.receive_byte();

        if (response == SEND_ERROR_MESSAGE) {
            std::string error_msg = protocol.receive_error_message();
            throw std::runtime_error(error_msg);
        }
        if (lobby_action == SEND_CREATE_GAME){
            std::cout << "[CLIENT] Waiting for other player to start: " << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[CLIENT] Error sending config: " << e.what() << std::endl;
        throw;
    }

}

void Client::run() {

    std::string map_path;
    float spawn_x, spawn_y;
    try {
        protocol.receive_game_init_data(map_path, spawn_x, spawn_y);
    } catch (const std::exception& e) {
        std::cerr << "[CLIENT] Error receiving game init data: " << e.what() << std::endl;
        return;
    }

    ClientReceiver receiver(protocol);
    receiver.start();
    Queue<Command> command_queue;
    ClientSender sender(protocol, command_queue);
    sender.start();

    Snapshot snapshot;
    InputParser parser(sender, command_queue);

    while (!receiver.try_pop_car_state(snapshot)) {
        SDL_Delay(10);
    }

    ClientHandler handler(parser);

    GraphicClient graphic_client(snapshot, &handler);
    
    const Uint32 FRAME_DELAY = 1000 / 60;



    while (true) {
        Uint32 frame_start = SDL_GetTicks();

        if (receiver.has_final_results()) {
            std::cout << "[CLIENT] Final results received automatically!" << std::endl;
            final_results = receiver.get_final_results();
            final_results_received = true;
            break;
        }
        while (receiver.try_pop_car_state(snapshot)) {
            graphic_client.update_from_snapshot(snapshot);
        }
        

        try {
            handler.handle_event();
        } catch (ClientQuitException& e) {
            break;
        }

        graphic_client.draw(snapshot);

        Uint32 frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frame_time);
        }
    }



    command_queue.close();
    protocol.close();
    receiver.stop();
    receiver.join();
    sender.stop();
    sender.join();
    SDL_Quit();
}

bool Client::has_final_results() const {
    return final_results_received;
}

FinalScoreList Client::get_final_results() const {
    return final_results;
}