#include "client.h"
#include "client_protocol.h"
#include "client_receiver.h"
#include "client_sender.h"
#include <iostream>
#include "client_handler.h"
#include "../common/queue.h"
#include "client_car.h"
#include "ClientQuitException.h"
#include "graphic_client.h"

Client::Client(const std::string& host, const std::string& port, AudioManager* audio)
    : host(host), port(port), client_socket(host.c_str(), port.c_str()), protocol(client_socket), final_results_received(false), audio_manager(audio)  {}

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
    if (!initialize_game_data()) {
        return;
    }

    ClientReceiver receiver(protocol);
    receiver.start();
    Queue<Command> command_queue;
    ClientSender sender(protocol, command_queue);
    sender.start();

    Snapshot snapshot;
    InputParser parser(sender, command_queue);

    if (!wait_for_initial_snapshot(receiver, snapshot)) {
        cleanup_resources(receiver, sender, command_queue);
        return;
    }
    
    AudioManager audio; 
    ClientHandler handler(parser);
    GraphicClient graphic_client(snapshot, &handler,&audio);

    game_loop(receiver, handler, graphic_client, snapshot);

    cleanup_resources(receiver, sender, command_queue);
}

bool Client::initialize_game_data() {
    std::string map_path;
    float spawn_x, spawn_y;
    try {
        protocol.receive_game_init_data(map_path, spawn_x, spawn_y);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[CLIENT] Error receiving game init data: " << e.what() << std::endl;
        return false;
    }
}

bool Client::wait_for_initial_snapshot(ClientReceiver& receiver, Snapshot& snapshot) {
    while (!receiver.try_pop_snapshot(snapshot)) {
        SDL_Delay(SNAPSHOT_POLL_DELAY_MS);
    }
    return true;
}

void Client::game_loop(ClientReceiver& receiver, ClientHandler& handler, 
                       GraphicClient& graphic_client, Snapshot& snapshot) {
    while (!should_exit()) {
        Uint32 frame_start = SDL_GetTicks();

        if (update_game_state(receiver, graphic_client, snapshot)) {
            break;
        }

        if (handle_input(handler)) {
            break;
        }

        render_frame(graphic_client, snapshot);
        limit_frame_rate(frame_start);
    }
}

bool Client::should_exit() const {
    return false;
}

bool Client::update_game_state(ClientReceiver& receiver, GraphicClient& graphic_client, 
                               Snapshot& snapshot) {
    if (receiver.has_final_results()) {
        std::cout << "[CLIENT] Final results received automatically!" << std::endl;
        final_results = receiver.get_final_results();
        final_results_received = true;
        return true;
    }

    while (receiver.try_pop_snapshot(snapshot)) {
        graphic_client.update_from_snapshot(snapshot);
    }
    return false;
}

bool Client::handle_input(ClientHandler& handler) {
    try {
        handler.handle_event();
        return false;
    } catch (ClientQuitException& e) {
        return true;
    }
}

void Client::render_frame(GraphicClient& graphic_client, Snapshot& snapshot) {
    graphic_client.draw(snapshot);
}

void Client::limit_frame_rate(Uint32 frame_start) {
    Uint32 frame_time = SDL_GetTicks() - frame_start;
    if (frame_time < FRAME_DELAY_MS) {
        SDL_Delay(FRAME_DELAY_MS - frame_time);
    }
}

void Client::cleanup_resources(ClientReceiver& receiver, ClientSender& sender, 
                               Queue<Command>& command_queue) {
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