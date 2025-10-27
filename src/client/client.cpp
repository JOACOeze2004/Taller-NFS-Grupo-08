#include "client.h"
#include "client_protocol.h"
#include "../common/constants.h"
#include "client_receiver.h"

#include <iostream> 
#include <SDL2/SDL.h>
#include "client_handler.h"
#include "../common/queue.h"
#include "client_car.h"
#include <SDL2/SDL_image.h>


Client::Client(const std::string& host, const std::string& port)
    : host(host), port(port), client_socket(host.c_str(), port.c_str()) {}

void Client::run() {
    std::cout << "[CLIENT] Connected to " << host << ":" << port << std::endl;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "[CLIENT] Error inicializando SDL: " << SDL_GetError() << std::endl;
        return ;
    }

    
    SDL_Window* window = SDL_CreateWindow(
        "Client Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1200, 900,
        SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED
    );


    if (!window) {
        std::cerr << "[CLIENT] Error creando ventana: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "[CLIENT] Error creando renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    ClientProtocol protocol(client_socket);
    
    ClientReceiver receiver(protocol); //Supongo q esta mal q esto este aca y q hagamos start.
    receiver.start();

    
    Queue<Command> command_queue;
    ClientSender sender(protocol, command_queue); 
    sender.start();

    bool running = true;
    std::string line;
    CarDTO state;

    InputParser parser(sender, command_queue);

    KeyboardHandler keyboard(parser);
    SDL_Event event;

    Car car(200, 200, renderer);

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    std::cerr << "Error inicializando SDL_image: " << IMG_GetError() << std::endl;
    }
    // Cargar imagen de fondo

    SDL_Surface* bg_surface = IMG_Load("../assets/need-for-speed/cities/Game Boy _ GBC - Grand Theft Auto - Backgrounds - San Andreas.png"); 
    if (!bg_surface) {
        std::cerr << "[CLIENT] Error cargando fondo: " << SDL_GetError() << std::endl;
    }
    SDL_Texture* bg_texture = SDL_CreateTextureFromSurface(renderer, bg_surface);
    SDL_FreeSurface(bg_surface);  // Liberar surface después de crear la texture

    if (!bg_texture) {
        std::cerr << "[CLIENT] Error creando textura: " << SDL_GetError() << std::endl;
    }

    while (running) {
        while (receiver.try_pop_car_state(state)) {
            car.update_from_dto(state);
        }
        
        // Procesar eventos
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN){
                if (event.key.keysym.sym == SDLK_q) {
                    running = false;
                }                
            }           
            keyboard.process_event(event);
        }
        keyboard.update();
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderClear(renderer);
        
        if (bg_texture) {
        SDL_RenderCopy(renderer, bg_texture, NULL, NULL);  // Dibujar fondo completo
        }

        car.render();  
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16); 
    }
    

    receiver.stop();
    receiver.join();
    protocol.close();
    SDL_DestroyWindow(window);
    SDL_Quit();
}
