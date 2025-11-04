#include "graphic_client.h"
#include <iostream>
#include <SDL2/SDL.h>
#include "client_handler.h"
#include "client_car.h"
#include <SDL2/SDL_image.h>

GraphicClient::GraphicClient(const std::string& map_path, const DTO& initial_dto)
    : renderer(nullptr), bg_texture(nullptr), window(nullptr), 
      player_car_id(-1), camera_x(0.0f), camera_y(0.0f), 
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

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
        map_width = static_cast<float>(bg_surface->w);
        map_height = static_cast<float>(bg_surface->h);
    }
    
    bg_texture = SDL_CreateTextureFromSurface(renderer, bg_surface);
    SDL_FreeSurface(bg_surface);

    if (!bg_texture) {
        std::cerr << "[CLIENT] Error creando textura: " << SDL_GetError() << std::endl;
    }

    set_player_car(initial_dto.id);

    for (const auto& [id, car_state] : initial_dto.cars) {
        update_car(id, car_state);
    }
    set_player_car(initial_dto.id);

    update_camera();
    
    draw();
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

    camera_x = (player_car.x - static_cast<float>(screen_width) / 2.0f);
    camera_y = (player_car.y - static_cast<float>(screen_height) / 2.0f);

    if (camera_x < 0.0f) camera_x = 0.0f;
    if (camera_y < 0.0f) camera_y = 0.0f;
    if (camera_x > map_width - static_cast<float>(screen_width)) 
        camera_x = map_width - static_cast<float>(screen_width);
    if (camera_y > map_height - static_cast<float>(screen_height)) 
        camera_y = map_height - static_cast<float>(screen_height);
}

void GraphicClient::draw() {
    update_camera();

    /* SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer); */

    if (bg_texture) {
        SDL_Rect src_rect = {
            static_cast<int>(camera_x), 
            static_cast<int>(camera_y), 
            screen_width,
            screen_height
        };
        SDL_FRect dst_rect = {
            0.0f, 
            0.0f, 
            static_cast<float>(screen_width), 
            static_cast<float>(screen_height)
        };
        SDL_RenderCopyF(renderer, bg_texture, &src_rect, &dst_rect);
    }

    draw_cars();
    
    draw_minimap();
    
    SDL_RenderPresent(renderer);
}

void GraphicClient::draw_minimap() {
    if (!bg_texture) return;

    const float minimap_scale = 0.04f;  
    const int minimap_width = static_cast<int>(map_width * minimap_scale);
    const int minimap_height = static_cast<int>(map_height * minimap_scale);
    const int minimap_x = screen_width - minimap_width - 10;  
    const int minimap_y = 10;  

    const float radar_radius = 400.0f;
    
    if (player_car_id < 0 || cars.find(player_car_id) == cars.end()) {
        return;
    }
    
    const CarDTO& player_car = cars[player_car_id];
    
    SDL_Rect src_rect = {
        static_cast<int>(player_car.x - radar_radius),
        static_cast<int>(player_car.y - radar_radius),
        static_cast<int>(radar_radius * 2.0f),
        static_cast<int>(radar_radius * 2.0f)
    };
    
    if (src_rect.x < 0) src_rect.x = 0;
    if (src_rect.y < 0) src_rect.y = 0;
    if (src_rect.x + src_rect.w > static_cast<int>(map_width)) 
        src_rect.x = static_cast<int>(map_width) - src_rect.w;
    if (src_rect.y + src_rect.h > static_cast<int>(map_height)) 
        src_rect.y = static_cast<int>(map_height) - src_rect.h;

    SDL_FRect dst_rect = {
        static_cast<float>(minimap_x), 
        static_cast<float>(minimap_y), 
        static_cast<float>(minimap_width), 
        static_cast<float>(minimap_height)
    };
    
    SDL_RenderCopyF(renderer, bg_texture, &src_rect, &dst_rect);

    const float scale = static_cast<float>(minimap_width) / static_cast<float>(src_rect.w);

    for (const auto& [id, car] : cars) {
        if (car.x >= src_rect.x && car.x <= src_rect.x + src_rect.w &&
            car.y >= src_rect.y && car.y <= src_rect.y + src_rect.h) {
            
            float car_minimap_x = minimap_x + (car.x - src_rect.x) * scale - 3.0f;
            float car_minimap_y = minimap_y + (car.y - src_rect.y) * scale - 3.0f;

            SDL_FRect car_rect = {car_minimap_x, car_minimap_y, 6.0f, 6.0f};
            
            if (id == player_car_id) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  
            }
            SDL_RenderFillRectF(renderer, &car_rect);
        }
    }
    
    
    SDL_FRect border_rect = {
        static_cast<float>(minimap_x), 
        static_cast<float>(minimap_y), 
        static_cast<float>(minimap_width), 
        static_cast<float>(minimap_height)
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRectF(renderer, &border_rect);
}

void GraphicClient::draw_cars() {
    for (const auto& [id, car] : cars) {
        if (car.x >= camera_x && car.x <= camera_x + static_cast<float>(screen_width) &&
            car.y >= camera_y && car.y <= camera_y + static_cast<float>(screen_height)) {
            draw_car(car);
        }
    }
}

void GraphicClient::draw_car(const CarDTO& car) {
    Car temp_car(car.x - camera_x, car.y - camera_y, car.angle, renderer);
    CarDTO adjusted_car = car;
    adjusted_car.x = car.x - camera_x;
    adjusted_car.y = car.y - camera_y;
    temp_car.update_from_dto(adjusted_car);
    temp_car.render();
}

GraphicClient::~GraphicClient() {
    if (bg_texture) {
        SDL_DestroyTexture(bg_texture);
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    IMG_Quit();
    SDL_Quit();
}
