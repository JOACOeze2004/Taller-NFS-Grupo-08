#include "client.h"
#include "client_protocol.h"
#include "../common/constants.h"
#include "client_receiver.h"

#include <iostream> 
#include <SDL2/SDL.h>
#include "client_handler.h"
#include "../common/queue.h"


Client::Client(const std::string& host, const std::string& port)
    : host(host), port(port), client_socket(host.c_str(), port.c_str()) {}

void Client::run() {
    std::cout << "[CLIENT] Connected to " << host << ":" << port << std::endl;

    // Inicializar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "[CLIENT] Error inicializando SDL: " << SDL_GetError() << std::endl;
        return ;
    }

    // Crear ventana SDL
    SDL_Window* window = SDL_CreateWindow(
        "Client Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "[CLIENT] Error creando ventana: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    ClientProtocol protocol(client_socket);
    
    ClientReceiver receiver(protocol); //Supongo q esta mal q esto este aca y q hagamos start.
    receiver.start();

    //crear queue
    Queue<Command> command_queue;
    ClientSender sender(protocol, command_queue); 
    sender.start();

    bool running = true;
    std::string line;
    CarDTO state;

    InputParser parser(sender, command_queue);

    KeyboardHandler keyboard(parser);
    SDL_Event event;

    while (running) {
        while (receiver.try_pop_car_state(state)) {
            std::cout << "[CLIENT] Car x:" << state.x
                      << " y:" << state.y
                      << " vel:" << state.velocity
                      << " ang:" << state.angle << std::endl;
        }
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            keyboard.process_event(event);
        }
        SDL_Delay(16);
    }
    

    receiver.stop();
    receiver.join();
    protocol.close();
    SDL_DestroyWindow(window);
    SDL_Quit();
}
