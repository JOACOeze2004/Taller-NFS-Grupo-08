#include "graphic_client.h"
#include <iostream>
#include <SDL2/SDL.h>
#include "client_handler.h"
#include "client_car.h"
#include <SDL2/SDL_image.h>
#include <cmath>

constexpr float ZOOM_FACTOR = 2.0f;

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
    
    draw(initial_dto);
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

    camera_x = (player_car.x - static_cast<float>(screen_width) / (2.0f * ZOOM_FACTOR));
    camera_y = (player_car.y - static_cast<float>(screen_height) / (2.0f * ZOOM_FACTOR));

    const float viewport_width = static_cast<float>(screen_width) / ZOOM_FACTOR;
    const float viewport_height = static_cast<float>(screen_height) / ZOOM_FACTOR;

    if (camera_x < 0.0f) camera_x = 0.0f;
    if (camera_y < 0.0f) camera_y = 0.0f;
    if (camera_x > map_width - viewport_width) 
        camera_x = map_width - viewport_width;
    if (camera_y > map_height - viewport_height) 
        camera_y = map_height - viewport_height;

}


void GraphicClient::draw_camera(){
    if (bg_texture) {
        const float viewport_width = static_cast<float>(screen_width) / ZOOM_FACTOR;
        const float viewport_height = static_cast<float>(screen_height) / ZOOM_FACTOR;

        const int src_x = static_cast<int>(std::floor(camera_x));
        const int src_y = static_cast<int>(std::floor(camera_y));
        const int src_w = static_cast<int>(std::ceil(viewport_width)) + 1;
        const int src_h = static_cast<int>(std::ceil(viewport_height)) + 1;

        SDL_Rect src_rect = {src_x, src_y, src_w, src_h};

        int texW = 0, texH = 0;
        if (SDL_QueryTexture(bg_texture, nullptr, nullptr, &texW, &texH) == 0) {
            if (src_rect.x < 0) { 
                src_rect.w += src_rect.x; 
                src_rect.x = 0; 
            }
            if (src_rect.y < 0) { 
                src_rect.h += src_rect.y; 
                src_rect.y = 0; 
            }
            if (src_rect.x + src_rect.w > texW) 
                src_rect.w = texW - src_rect.x;
            if (src_rect.y + src_rect.h > texH) 
                src_rect.h = texH - src_rect.y;
        }

        const float fx = camera_x - static_cast<float>(src_x);
        const float fy = camera_y - static_cast<float>(src_y);

        SDL_FRect dst_rect = {
            -fx * ZOOM_FACTOR,
            -fy * ZOOM_FACTOR,
            static_cast<float>(screen_width) + ZOOM_FACTOR,
            static_cast<float>(screen_height) + ZOOM_FACTOR
        };
        
        SDL_RenderCopyF(renderer, bg_texture, &src_rect, &dst_rect);
    }
}

void GraphicClient::clear_cars(const std::unordered_map<int, CarDTO>& cars_in_dto) {
    for (auto it = cars.begin(); it != cars.end(); ) {
        if (cars_in_dto.find(it->first) == cars_in_dto.end()) {
            it = cars.erase(it);
        } else {
            ++it;
        }
    }    
}

void GraphicClient::draw(const DTO& dto) {
    
    update_camera();

    clear_cars(dto.cars);

    draw_camera();

    draw_cars();
    
    draw_minimap();

    /* draw_position(dto.position, dto.cars.size());

    draw_time(dto.time_ms);

    if (player_car_id >= 0 && cars.find(player_car_id) != cars.end()) {
        draw_speed(cars[player_car_id].velocity);
    } */
    
    SDL_RenderPresent(renderer);
}

void GraphicClient::draw_position(int position, int total_cars) {
    std::cout << "Position: " << position << " / " << total_cars << std::endl;
    //mientras lo dejo asi, falta agregar sdlthings
}

void GraphicClient::draw_time(int time_ms) {
    int total_seconds = time_ms / 1000;
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;
    int milliseconds = time_ms % 1000;

    std::cout << "Time: " << minutes << ":" 
              << (seconds < 10 ? "0" : "") << seconds << "." 
              << milliseconds << std::endl;
    //mientras lo dejo asi, falta agregar sdlthings
}

void GraphicClient::draw_speed(float speed) {
    std::cout << "Speed: " << speed << " units/s" << std::endl;
    //mientras lo dejo asi, falta agregar sdlthings seguro haya que pasar la velocidad debox2d a algo razonable
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
    const float viewport_width = static_cast<float>(screen_width) / ZOOM_FACTOR;
    const float viewport_height = static_cast<float>(screen_height) / ZOOM_FACTOR;

    for (const auto& [id, car] : cars) {
        if (car.x >= camera_x && car.x <= camera_x + viewport_width &&
            car.y >= camera_y && car.y <= camera_y + viewport_height) {
            draw_car(car);
        }
    }
}

void GraphicClient::draw_car(const CarDTO& car) {
    const float screen_x = (car.x - camera_x) * ZOOM_FACTOR;
    const float screen_y = (car.y - camera_y) * ZOOM_FACTOR;
    
    Car temp_car(screen_x, screen_y, car.angle, renderer, ZOOM_FACTOR);
    CarDTO adjusted_car = car;
    adjusted_car.x = screen_x;
    adjusted_car.y = screen_y;
    temp_car.update_from_dto(adjusted_car);
    temp_car.render();  //aca tengo que cambiar la implementacion para no usar un aux
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
