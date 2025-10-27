#include "client.h"
#include "client_protocol.h"
#include "client_receiver.h"

#include <iostream> 
#include <SDL2/SDL.h>
#include "client_handler.h"
#include "../common/queue.h"
#include "client_car.h"
#include "ClientQuitException.h"
#include "graphic_client.h"


Client::Client(const std::string& host, const std::string& port)
    : host(host), port(port), client_socket(host.c_str(), port.c_str()) {}

void Client::run() {
    std::cout << "[CLIENT] Connected to " << host << ":" << port << std::endl;

    ClientProtocol protocol(client_socket);
    ClientReceiver receiver(protocol);
    receiver.start();
    Queue<Command> command_queue;
    ClientSender sender(protocol, command_queue);
    sender.start();

    CarDTO state;
    InputParser parser(sender, command_queue);
    GraphicClient graphic_client;
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
