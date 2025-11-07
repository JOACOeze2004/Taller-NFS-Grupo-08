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

void Client::run(const PlayerConfig& config,uint8_t lobby_action, const std::string& game_id) {
    std::cout << "[CLIENT] Connected to " << host << ":" << port << std::endl;

    ClientProtocol protocol(client_socket);

    try {
        protocol.send_player_config(config.playerName, config.carId, config.mapName);        
        protocol.send_lobby_action(lobby_action, game_id);
        if (lobby_action == SEND_CREATE_GAME){
            std::cout << "[CLIENT] Waiting for other player to start: " << std::endl;
        }        
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

    Snapshot snopshot;
    InputParser parser(sender, command_queue);

    while (!receiver.try_pop_car_state(snopshot)) {
        SDL_Delay(10);
    }

    /* 
        while(!receiver.try_pop_snapshot(snapshot)) {
            SDL_Delay(10);
        }
    */

    /* 
        GraphicClient graphic_client(map_path, snapshot);
    */
    GraphicClient graphic_client(map_path,snopshot);
    ClientHandler handler(parser);
    const Uint32 FRAME_DELAY = 1000 / 60;  // ~60 FPS



    while (true) {
        Uint32 frame_start = SDL_GetTicks();

        while (receiver.try_pop_car_state(snopshot)) {
            for (auto& [id, car] : snopshot.cars) {
                graphic_client.update_car(id, car);
            }
        }
        
        /* while(!receiver.try_pop_snapshot(snapshot)) {
            // Condiciones tipo mori hacer algo, o lo que tenga que ver con juego y no con mostrar cosas
            graphic_client.update_from_snapshot(snapshot);
            } */

        try {
            handler.handle_event();
        } catch (ClientQuitException& e) {
            break;
        }

        graphic_client.draw(snopshot);
        /* graphic_client.draw(snapshot); */

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
