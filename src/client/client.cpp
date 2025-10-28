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
    : host(host), port(port), client_socket(host.c_str(), port.c_str()) {}

void Client::run(const PlayerConfig& config) {
    std::cout << "[CLIENT] Connected to " << host << ":" << port << std::endl;

    ClientProtocol protocol(client_socket);

    try {
        protocol.send_player_config(config.playerName, config.carId, config.mapName);
    } catch (const std::exception& e) {
        std::cerr << "[CLIENT] Error sending config: " << e.what() << std::endl;
        return;
    }

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

    CarDTO state;
    InputParser parser(sender, command_queue);
    GraphicClient graphic_client(map_path);
    ClientHandler handler(parser);

    while (true) {
        while (receiver.try_pop_car_state(state)) {
            graphic_client.update_car(state);
        }

        try {
            handler.handle_event();
        } catch (ClientQuitException& e) {
            break;
        }
        graphic_client.draw();
    }
    command_queue.close();
    receiver.stop();
    receiver.join();
    sender.stop();
    sender.join();
    protocol.close();
    SDL_Quit();
}
