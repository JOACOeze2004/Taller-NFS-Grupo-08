#include "graphic_client.h"
#include <iostream>
#include <SDL2/SDL.h>
#include "client_handler.h"
#include "client_car.h"
#include <SDL2/SDL_image.h>

GraphicClient::GraphicClient(const std::string& map_path)
    : renderer(nullptr), bg_texture(nullptr), window(nullptr), 
      player_car_id(-1), camera_x(0), camera_y(0), 
      screen_width(1200), screen_height(900) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "[CLIENT] Error inicializando SDL: " << SDL_GetError() << std::endl;
        return;
    }

    window = SDL_CreateWindow("Client Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                              screen_width, screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED);

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
    } else {
        map_width = bg_surface->w;
        map_height = bg_surface->h;
    }
    
    bg_texture = SDL_CreateTextureFromSurface(renderer, bg_surface);
    SDL_FreeSurface(bg_surface);

    if (!bg_texture) {
        std::cerr << "[CLIENT] Error creando textura: " << SDL_GetError() << std::endl;
    }

    
}

void GraphicClient::set_player_car(int id) {
    player_car_id = id;
}

void GraphicClient::update_car(int id, const CarDTO& car_state) {
    cars[id] = car_state; 
}

void GraphicClient::update_camera() {
    if (player_car_id < 0 || cars.find(player_car_id) == cars.end()) {
        return;
    }

    const CarDTO& player_car = cars[player_car_id];
    
    // Centrar cámara en el coche del jugador
    camera_x = player_car.x - screen_width / 2;
    camera_y = player_car.y - screen_height / 2;
    
    // Limitar la cámara a los bordes del mapa
    if (camera_x < 0) camera_x = 0;
    if (camera_y < 0) camera_y = 0;
    if (camera_x > map_width - screen_width) camera_x = map_width - screen_width;
    if (camera_y > map_height - screen_height) camera_y = map_height - screen_height;
}

void GraphicClient::draw() {
    // Actualizar cámara ANTES de renderizar (mientras cars todavía tiene datos)
    update_camera();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (bg_texture) {
        // Definir qué parte del mapa se muestra
        SDL_Rect src_rect = {camera_x, camera_y, screen_width, screen_height};
        SDL_Rect dst_rect = {0, 0, screen_width, screen_height};
        SDL_RenderCopy(renderer, bg_texture, &src_rect, &dst_rect);
    }
    
    // Renderizar coches con posiciones ajustadas a la cámara
    for (const auto& [id, car] : cars) {
        // Solo renderizar si está en el viewport
        if (car.x >= camera_x && car.x <= camera_x + screen_width &&
            car.y >= camera_y && car.y <= camera_y + screen_height) {
            draw_car(car);
        }
    }
    
    SDL_RenderPresent(renderer);
}

void GraphicClient::draw_car(const CarDTO& car) {
    // Crear coche con posiciones ajustadas a la cámara
    Car temp_car(car.x - camera_x, car.y - camera_y, renderer);
    CarDTO adjusted_car = car;
    adjusted_car.x = car.x - camera_x;
    adjusted_car.y = car.y - camera_y;
    temp_car.update_from_dto(adjusted_car);
    temp_car.render();
}

GraphicClient::~GraphicClient() {
    SDL_DestroyWindow(window);
}
