#include "graphic_client.h"
#include <iostream>
#include <SDL2/SDL.h>
#include "client_handler.h"
#include "client_car.h"
#include <SDL2/SDL_image.h>

GraphicClient::GraphicClient(const std::string& map_path)
    : renderer(nullptr), bg_texture(nullptr), window(nullptr){

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "[CLIENT] Error inicializando SDL: " << SDL_GetError() << std::endl;
        return;
    }

    window = SDL_CreateWindow("Client Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200,
                              900, SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED);

    if (!window) {
        std::cerr << "[CLIENT] Error creando ventana: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "[CLIENT] Error creando renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "Error inicializando SDL_image: " << IMG_GetError() << std::endl;
    }

    SDL_Surface* bg_surface = IMG_Load(map_path.c_str());

    if (!bg_surface) {
        std::cerr << "[CLIENT] Error cargando fondo: " << SDL_GetError() << std::endl;
    }
    bg_texture = SDL_CreateTextureFromSurface(renderer, bg_surface);
    SDL_FreeSurface(bg_surface);

    if (!bg_texture) {
        std::cerr << "[CLIENT] Error creando textura: " << SDL_GetError() << std::endl;
    }

    
}

void GraphicClient::update_car(int id, const CarDTO& car_state) {
    cars[id] = car_state; 
}

void GraphicClient::draw() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (bg_texture) {
        SDL_RenderCopy(renderer, bg_texture, NULL, NULL);
    }
    
    //aca va a haber que filtrar de si esta en la pantalla
    for (const auto& [id, car] : cars) {
        draw_car(car);
    }
    
    SDL_RenderPresent(renderer);
    cars.clear();
    SDL_Delay(16);
}

void GraphicClient::draw_car(const CarDTO& car) {
    Car temp_car(car.x, car.y, renderer);
    temp_car.update_from_dto(car);
    temp_car.render();
}

GraphicClient::~GraphicClient() {
    SDL_DestroyWindow(window);
}
