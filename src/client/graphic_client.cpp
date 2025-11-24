#include "graphic_client.h"
#include <iostream>
#include <SDL2/SDL.h>
#include "client_handler.h"
#include "client_car.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include "text_renderer.h"
#include "upgrade_phase.h"
#include "audio_manager.h"

GraphicClient::GraphicClient(const Snapshot& initial_snapshot, ClientHandler* _handler, AudioManager* audio)
        : renderer(nullptr), bg_texture(nullptr), window(nullptr),
            player_car_id(-1), camera_x(0.0f), camera_y(0.0f), 
            screen_width(DEFAULT_SCREEN_WIDTH), screen_height(DEFAULT_SCREEN_HEIGHT), 
            text(nullptr), handler(_handler), 
            ready_sent(false), upgrade_phase(nullptr), audio_manager(audio),
            previous_collision(NONE_COLLISION), previous_using_nitro(false), previous_checkpoint_count(0) {

    initialize_sdl();
    initialize_window();
    initialize_renderer();
    initialize_image_and_ttf();
    initialize_resource_manager();
    load_core_textures();
    load_map_texture(initial_snapshot.map);
    initialize_text_renderer();
    
    set_player_car(initial_snapshot.id);
    update_from_snapshot(initial_snapshot);
    camera_id = player_car_id;
    
    initialize_upgrade_phase();
    draw(initial_snapshot);
}

void GraphicClient::initialize_sdl() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error(
            std::string("[CLIENT] Error inicializando SDL: ") + SDL_GetError()
        );
    }
}

void GraphicClient::initialize_window() {
    window = SDL_CreateWindow("Client Game", 
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              screen_width, screen_height, 
                              SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED);
    
    if (!window) {
        SDL_Quit();
        throw std::runtime_error(
            std::string("[CLIENT] Error creando ventana: ") + SDL_GetError()
        );
    }
}

void GraphicClient::initialize_renderer() {
    renderer = SDL_CreateRenderer(window, -1, 
                                  SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error(
            std::string("[CLIENT] Error creando renderer: ") + SDL_GetError()
        );
    }
}

void GraphicClient::initialize_image_and_ttf() {
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "[CLIENT] Warning: SDL_image PNG support: " 
                  << IMG_GetError() << std::endl;
    }
    
    if (TTF_Init() == -1) {
        throw std::runtime_error(
            std::string("[CLIENT] Error inicializando SDL_ttf: ") + TTF_GetError()
        );
    }
}

void GraphicClient::initialize_resource_manager() {
    resources = std::make_unique<ResourceManager>(renderer);
}

void GraphicClient::load_core_textures() {
    checkpoint_texture = resources->load(CHECKPOINT_TEXTURE_PATH);
    hint_texture = resources->load(HINT_TEXTURE_PATH);
    speedometer_texture = resources->load(SPEEDOMETER_TEXTURE_PATH);
}

void GraphicClient::load_map_texture(int map_id) {
    std::string map_path;
    switch (map_id) {
        case LIBERTY_CITY:
            map_path = LIBERTY_CITY_MAP_PATH;
            break;
        case SAN_ANDREAS:
            map_path = SAN_ANDREAS_MAP_PATH;
            break;
        case VICE_CITY:
            map_path = VICE_CITY_MAP_PATH;
            break;
        default:
            throw std::runtime_error(
                "[CLIENT] Invalid map ID: " + std::to_string(map_id)
            );
    }
    
    SDL_Surface* bg_surface = IMG_Load(map_path.c_str());
    if (!bg_surface) {
        throw std::runtime_error(
            std::string("[CLIENT] Error cargando fondo: ") + SDL_GetError()
        );
    }
    
    map_width = static_cast<float>(bg_surface->w);
    map_height = static_cast<float>(bg_surface->h);
    SDL_FreeSurface(bg_surface);
    
    bg_texture = resources->load(map_path.c_str());
}

void GraphicClient::initialize_text_renderer() {
    text = std::make_unique<TextRenderer>(DEFAULT_FONT_PATH, DEFAULT_FONT_SIZE);
    if (!text->ok()) {
        std::cerr << "[CLIENT] Fuente no disponible, no se renderizará texto en pantalla." << std::endl;
        text.reset();
    }
}

void GraphicClient::initialize_upgrade_phase() {
    upgrade_phase = std::make_unique<UpgradePhase>(
        renderer, window, screen_width, screen_height, handler, resources.get()
    );
}

void GraphicClient::update_from_snapshot(const Snapshot& snapshot) {

    for (auto& [id, car] : snapshot.cars) {
        update_car(id, car);
    }

    update_camera();

    clear_cars(snapshot.cars);
    
    auto player_it = snapshot.cars.find(player_car_id);
    if (player_it != snapshot.cars.end() && player_it->second.state != IN_GAME) {
        for (const auto& [id, car_state] : snapshot.cars) {
            if (car_state.state == IN_GAME && id != player_car_id) {
               camera_id = id;
            }
        }
    } else {
        camera_id = player_car_id;
    }
    
    if (audio_manager && player_it != snapshot.cars.end()) {
        const CarDTO& player_car = player_it->second;

        //Detect transition from lobby to race and play start sound once
        static int previous_state = snapshot.state;
        if (previous_state == IN_LOBBY && snapshot.state == IN_RACE) {
            audio_manager->playSoundEffect(SoundEffect::RACE_START);
        }
        previous_state = snapshot.state;
        
        if (snapshot.collision != NONE_COLLISION && previous_collision == NONE_COLLISION) {
            if (snapshot.collision == HEAVY_COLLISION) {
                audio_manager->playSoundEffect(SoundEffect::CRASH);
            }
        }
        previous_collision = snapshot.collision;
        
        bool using_nitro = player_car.remaining_nitro > 0.0f && player_car.nitro > 0.0f;
        if (using_nitro && !previous_using_nitro) {
            audio_manager->playSoundEffect(SoundEffect::NITRO);
        }
        previous_using_nitro = using_nitro;
        
        if (player_car.state == DEAD) {
            audio_manager->playSoundEffect(SoundEffect::DEATH);
        }
        
        if (player_car.state == FINISHED && snapshot.state == IN_RACE) {
            audio_manager->playSoundEffect(SoundEffect::WIN);
        }
    }
}

void GraphicClient::set_player_car(int id) { 
    player_car_id = id;
}

void GraphicClient::update_car(int id, const CarDTO& car_state) {
    cars[id] = car_state;
    if (car_objects.find(id) == car_objects.end()) {
        car_objects[id] = std::make_unique<Car>(0.0f, 0.0f, 0.0f, renderer, resources.get(), car_state.car_id, ZOOM_FACTOR);
    }
}

void GraphicClient::update_camera() {
    if (camera_id < 0 || cars.find(camera_id) == cars.end()) {
        return;
    }

    const CarDTO& player_car = cars[camera_id];

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
            int removed_id = it->first;
            it = cars.erase(it);
            car_objects.erase(removed_id);
        } else {
            ++it;
        }
    }    
}

void GraphicClient::draw(const Snapshot& snapshot) {
    switch (snapshot.state) {
        case IN_WORK_SHOP:
            draw_workshop_state(snapshot);
            break;
        case IN_LOBBY:
            draw_lobby_state(snapshot);
            break;
        case IN_RACE:
            draw_race_state(snapshot);
            break;
        default:
            break;
    }
    SDL_RenderPresent(renderer);
}

void GraphicClient::draw_workshop_state(const Snapshot& snapshot) {
    auto player_it = snapshot.cars.find(player_car_id);
    int remaining_upgrades = (player_it != snapshot.cars.end()) ? player_it->second.remaining_upgrades : 0;
    upgrade_phase->render(remaining_upgrades);
}

void GraphicClient::draw_lobby_state(const Snapshot& snapshot) {
    draw_camera();
    draw_cars();
    draw_minimap(snapshot.checkpoint, snapshot.type_checkpoint, snapshot.hint);
    draw_speedometer(cars[player_car_id].velocity);
    draw_game_id(snapshot.game_id);
    
    if (snapshot.is_owner) {
        draw_ready_btn(static_cast<int>(snapshot.cars.size()), ready_sent);
    }
}

void GraphicClient::draw_race_state(const Snapshot& snapshot) {
    draw_camera();
    draw_checkpoint(snapshot.checkpoint, snapshot.type_checkpoint);
    draw_hint(snapshot.hint);
    draw_minimap(snapshot.checkpoint, snapshot.type_checkpoint, snapshot.hint);
    draw_speedometer(cars[player_car_id].velocity);
    draw_game_id(snapshot.game_id);
    draw_cars();
    
    auto player_it = snapshot.cars.find(player_car_id);
    if (player_it != snapshot.cars.end() && player_it->second.state != IN_GAME) {
        draw_state(player_it->second.state);
        draw_results(snapshot.cars_finished);
    } else {
        draw_position(snapshot.position, snapshot.cars.size());
        draw_time(snapshot.time_ms);
    }
} 

void GraphicClient::draw_results(const std::vector<CarRacingInfo>& cars_finished) {

    if (!text || cars_finished.empty()) return;

    const int panel_w = 800;  
    const int panel_h = 600;  
    const int panel_x = (screen_width - panel_w) / 2;
    const int panel_y = (screen_height - panel_h) / 2;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 230);
    SDL_Rect panel_rect = {panel_x, panel_y, panel_w, panel_h};
    SDL_RenderFillRect(renderer, &panel_rect);

    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    SDL_RenderDrawRect(renderer, &panel_rect);

    SDL_Color title_color = {255, 215, 0, 255};
    std::string title = "RACE RESULTS";
    int title_x = panel_x + (panel_w - static_cast<int>(title.size()) * 14) / 2;
    int title_y = panel_y + 30;
    text->render(renderer, title, title_x, title_y, title_color);

    SDL_Color header_color = {200, 200, 200, 255};
    text->render(renderer, "Pos", panel_x + 30, panel_y + 80, header_color);
    text->render(renderer, "Name", panel_x + 120, panel_y + 80, header_color);
    text->render(renderer, "Time", panel_x + panel_w - 180, panel_y + 80, header_color);

    int row_y = panel_y + 120;
    const int row_height = 40;
    const size_t max_rows = 15; 

    for (size_t i = 0; i < cars_finished.size() && i < max_rows; ++i) {
        const auto& car_info = cars_finished[i];

        SDL_Color row_color = {255, 255, 255, 255};
        if (car_info.position == 1) {
            row_color = {255, 215, 0, 255};
        } else if (car_info.position == 2) {
            row_color = {192, 192, 192, 255};
        } else if (car_info.position == 3) {
            row_color = {205, 127, 50, 255};
        }

        char position_str[16];
        std::snprintf(position_str, sizeof(position_str), "%d.", car_info.position);
        text->render(renderer, position_str, panel_x + 30, row_y, row_color);

        std::string display_name = car_info.name;
        if (display_name.length() > 30) {
            display_name = display_name.substr(0, 27) + "...";
        }
        text->render(renderer, display_name, panel_x + 120, row_y, row_color);

        int total_seconds = static_cast<int>(car_info.time);
        int minutes = total_seconds / 60;
        int seconds = total_seconds % 60;
        int millis = static_cast<int>((car_info.time - total_seconds) * 1000);

        char time_str[32];
        std::snprintf(time_str, sizeof(time_str), "%02d:%02d.%03d", minutes, seconds, millis);
        text->render(renderer, time_str, panel_x + panel_w - 180, row_y, row_color);

        row_y += row_height;
    }
}

void GraphicClient::draw_ready_btn(int player_count, bool& ready_sent){
    if (!text || !handler) return;

    const int btn_w = 250;
    const int btn_h = 50;
    const int btn_x = (screen_width / 2) - (btn_w / 2);
    const int btn_y = screen_height - 120;
    SDL_Rect btn_rect = { btn_x, btn_y, btn_w, btn_h };

    if (!ready_sent) {
        handler->clear_buttons();
        handler->register_button(btn_rect, BUTTON_READY); 
    } else {
        handler->clear_buttons();
    }

    bool is_hovered = handler->is_mouse_over_button(btn_rect);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if (ready_sent) {
        SDL_SetRenderDrawColor(renderer, 0, 180, 0, 200); 
    } else if (is_hovered) {
        SDL_SetRenderDrawColor(renderer, 50, 164, 255, 230);
    } else {
        SDL_SetRenderDrawColor(renderer, 30, 144, 255, 200);
    }
    SDL_RenderFillRect(renderer, &btn_rect);

    if (is_hovered && !ready_sent) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
    }
    SDL_RenderDrawRect(renderer, &btn_rect);

    SDL_Color label_color{255, 255, 255, 255};
    std::string label = ready_sent ? "READY (sent)" : "READY";
    text->render(renderer, label, btn_x + btn_w / 2 - 40, btn_y + btn_h / 2 -10, label_color);

    char buf[64];
    std::snprintf(buf, sizeof(buf), "Players: %i", player_count);
    text->render(renderer, buf, btn_x, btn_y - 28, label_color);
}

void GraphicClient::draw_state(int state) {
    if (!text) return; 
    std::string msg;
    SDL_Color overlay_color;
    
    if (state == FINISHED) {
        msg = "You Finished!";
        overlay_color = {0, 255, 0, 76}; 
    } else if (state == DEAD) {
        msg = "You Died!";
        overlay_color = {255, 0, 0, 76}; 
    } else if (state == TIMEOUTED) {
        msg = "Time Out!";
        overlay_color = {255, 0, 0, 76}; 
    } else {
        return; 
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, overlay_color.r, overlay_color.g, overlay_color.b, overlay_color.a);
    SDL_Rect fullscreen_rect = {0, 0, screen_width, screen_height};
    SDL_RenderFillRect(renderer, &fullscreen_rect);
    
    SDL_Color text_color{255, 255, 255, 255};
    int text_x = (screen_width / 2) - 100; 
    int text_y = (screen_height / 2) - 25; 
    text->render(renderer, msg, text_x, text_y, text_color);
}

void GraphicClient::draw_game_id(int id) {
    if (!text) return; 
    std::string msg = "GAME ID: " + std::to_string(id);
    SDL_Color color{255, 0, 0, 255};
    text->render(renderer, msg, screen_width - 150, 200, color);
}

void GraphicClient::draw_checkpoint(CheckpointCoords checkpoint, int type) {
    if (!checkpoint_texture) return;

    const float viewport_width = static_cast<float>(screen_width) / ZOOM_FACTOR;
    const float viewport_height = static_cast<float>(screen_height) / ZOOM_FACTOR;

    if (checkpoint.x >= camera_x && checkpoint.x <= camera_x + viewport_width &&
        checkpoint.y >= camera_y && checkpoint.y <= camera_y + viewport_height) {
        
        float screen_x = (checkpoint.x - camera_x) * ZOOM_FACTOR;
        float screen_y = (checkpoint.y - camera_y) * ZOOM_FACTOR;

        const float dst_w = 60.0f;
        const float dst_h = 90.0f;
        SDL_FRect dst_rect = {screen_x - dst_w * 0.5f, screen_y - dst_h * 0.5f, dst_w, dst_h};

        if (type == 1){
            SDL_Rect src_rect = {290, 650, 95, 120};
            SDL_RenderCopyF(renderer, checkpoint_texture, &src_rect, &dst_rect);
        }else if (type == 2){
            SDL_Rect src_rect = {410, 20, 90, 127};
            SDL_RenderCopyF(renderer, checkpoint_texture, &src_rect, &dst_rect);
        } else {
            SDL_Rect src_rect = {290, 175, 95, 120};
            SDL_RenderCopyF(renderer, checkpoint_texture, &src_rect, &dst_rect);
        }
    }    
} 

void GraphicClient::draw_hint(HintCoords hint) {
    if (!hint_texture) return;
    const float viewport_width = static_cast<float>(screen_width) / ZOOM_FACTOR;
    const float viewport_height = static_cast<float>(screen_height) / ZOOM_FACTOR;
    if (hint.x >= camera_x && hint.x <= camera_x + viewport_width &&
        hint.y >= camera_y && hint.y <= camera_y + viewport_height) {
        
        float screen_x = (hint.x - camera_x) * ZOOM_FACTOR;
        float screen_y = (hint.y - camera_y) * ZOOM_FACTOR;

        SDL_Rect src_rect = {260, 20, 120, 150};
        SDL_FRect dst_rect = {screen_x, screen_y, 40.0f, 60.0f};

        double angle_deg = hint.angle + 180;
        SDL_FPoint center = {15.0f, 15.0f}; 
                    
        SDL_RenderCopyExF(renderer, hint_texture, &src_rect, &dst_rect, angle_deg, &center, SDL_FLIP_NONE);
    }

}


void GraphicClient::draw_position(int position, int total_cars) {
    if (!text) return;
    std::string msg = "POSITION: " + std::to_string(position) + " / " + std::to_string(total_cars);
    SDL_Color color{255, 255, 255, 255};
    text->render(renderer, msg, 500, 30, color);
}

void GraphicClient::draw_time(int time_ms) {
    if (!text) return;
    int total_seconds = time_ms / 1000;
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;
    int milliseconds = time_ms % 1000;
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "TIME: %02d:%02d.%03d", minutes, seconds, milliseconds);
    SDL_Color color{255, 255, 255, 255};
    text->render(renderer, buffer, 500, 100, color);
}



void GraphicClient::draw_speedometer(float speed) {
    if (!speedometer_texture) return;
    SDL_Rect src_rect = {40, 70, 530, 240};
    SDL_Rect dst_rect = {75, screen_height - 175, 265, 140};
    SDL_RenderCopy(renderer, speedometer_texture, &src_rect, &dst_rect);


    SDL_Rect src_rect_needle = {270, 235, 61, 200};
    float angle = (speed / 400.0f) * 180.0f - 90.0f;
    SDL_FPoint center = {30.5f, 180.0f};
    SDL_FRect dst_rect_needle = {160, screen_height - 225.0f, 60.0f, 200.0f};
    SDL_RenderCopyExF(renderer, speedometer_texture, &src_rect_needle, &dst_rect_needle, angle, &center, SDL_FLIP_NONE);

}

void GraphicClient::draw_minimap(const CheckpointCoords& checkpoint, int checkpoint_type, const HintCoords& hint) {
    if (!bg_texture) return;

    const float minimap_scale = 0.04f;  
    const int minimap_width = static_cast<int>(map_width * minimap_scale);
    const int minimap_height = static_cast<int>(map_height * minimap_scale);
    const int minimap_x = screen_width - minimap_width - 10;  
    const int minimap_y = 10;  

    const float radar_radius = 400.0f;
    
    if (camera_id < 0 || cars.find(camera_id) == cars.end()) {
        return;
    }
    
    const CarDTO& camera_car = cars[camera_id];
    
    SDL_Rect src_rect = {
        static_cast<int>(camera_car.x - radar_radius),
        static_cast<int>(camera_car.y - radar_radius),
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
            car.y >= src_rect.y && car.y <= src_rect.y + src_rect.h && car.state == IN_GAME) {
            
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
    
    float checkpoint_minimap_x, checkpoint_minimap_y;
    bool is_inside = (checkpoint.x >= src_rect.x && checkpoint.x <= src_rect.x + src_rect.w &&
                        checkpoint.y >= src_rect.y && checkpoint.y <= src_rect.y + src_rect.h);
    
    if (is_inside) {
        checkpoint_minimap_x = minimap_x + (checkpoint.x - src_rect.x) * scale;
        checkpoint_minimap_y = minimap_y + (checkpoint.y - src_rect.y) * scale;
    } else {
        float center_x = camera_car.x;
        float center_y = camera_car.y;
        float dx = checkpoint.x - center_x;
        float dy = checkpoint.y - center_y;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance > 0) {
            dx /= distance;
            dy /= distance;
            checkpoint_minimap_x = minimap_x + minimap_width * 0.5f + dx * (minimap_width * 0.45f);
            checkpoint_minimap_y = minimap_y + minimap_height * 0.5f + dy * (minimap_height * 0.45f);
        } else {
            checkpoint_minimap_x = minimap_x + minimap_width * 0.5f;
            checkpoint_minimap_y = minimap_y + minimap_height * 0.5f;
        }
    }
    
    SDL_Color checkpoint_color;
    switch (checkpoint_type) {
        case 1: checkpoint_color = {0, 255, 0, 255}; break;   
        case 2: checkpoint_color = {255, 255, 0, 255}; break; 
        default: checkpoint_color = {255, 100, 0, 255}; break; 
    }
    
    const float checkpoint_size = is_inside ? 10.0f : 8.0f;
    SDL_FRect checkpoint_rect = {
        checkpoint_minimap_x - checkpoint_size * 0.5f,
        checkpoint_minimap_y - checkpoint_size * 0.5f,
        checkpoint_size,
        checkpoint_size
    };
    
    SDL_SetRenderDrawColor(renderer, checkpoint_color.r, checkpoint_color.g, checkpoint_color.b, 255);
    SDL_RenderFillRectF(renderer, &checkpoint_rect);
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRectF(renderer, &checkpoint_rect);
    
    SDL_FRect border_rect = {
        static_cast<float>(minimap_x), 
        static_cast<float>(minimap_y), 
        static_cast<float>(minimap_width), 
        static_cast<float>(minimap_height)
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    if (!hint_texture) {
        SDL_RenderDrawRectF(renderer, &border_rect);
        return;
    }
    
    float hint_minimap_x, hint_minimap_y;
    bool is_hint_inside = (hint.x >= src_rect.x && hint.x <= src_rect.x + src_rect.w &&
                        hint.y >= src_rect.y && hint.y <= src_rect.y + src_rect.h);
    
    if (is_hint_inside) {
        hint_minimap_x = minimap_x + (hint.x - src_rect.x) * scale;
        hint_minimap_y = minimap_y + (hint.y - src_rect.y) * scale;
    } else {
        float center_x = camera_car.x;
        float center_y = camera_car.y;
        float dx = hint.x - center_x;
        float dy = hint.y - center_y;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance > 0) {
            dx /= distance;
            dy /= distance;
            
            hint_minimap_x = minimap_x + minimap_width * 0.5f + dx * (minimap_width * 0.45f);
            hint_minimap_y = minimap_y + minimap_height * 0.5f + dy * (minimap_height * 0.45f);
        } else {
            hint_minimap_x = minimap_x + minimap_width * 0.5f;
            hint_minimap_y = minimap_y + minimap_height * 0.5f;
        }
    }
    
    SDL_Rect hint_src_rect = {260, 20, 120, 150};
    const float hint_arrow_w = is_hint_inside ? 16.0f : 12.0f;
    const float hint_arrow_h = is_hint_inside ? 24.0f : 18.0f;
    
    SDL_FRect hint_dst_rect = {
        hint_minimap_x - hint_arrow_w * 0.5f,
        hint_minimap_y - hint_arrow_h * 0.5f,
        hint_arrow_w,
        hint_arrow_h
    };
    
    double hint_angle_deg = hint.angle + 180;
    SDL_FPoint hint_center = {hint_arrow_w * 0.5f, hint_arrow_h * 0.5f};
    
    SDL_RenderCopyExF(renderer, hint_texture, &hint_src_rect, &hint_dst_rect, hint_angle_deg, &hint_center, SDL_FLIP_NONE);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRectF(renderer, &border_rect);
}


void GraphicClient::draw_cars() {
    const float viewport_width = static_cast<float>(screen_width) / ZOOM_FACTOR;
    const float viewport_height = static_cast<float>(screen_height) / ZOOM_FACTOR;

    for (const auto& [id, car_dto_world] : cars) {
        if (car_dto_world.x >= camera_x && car_dto_world.x <= camera_x + viewport_width &&
            car_dto_world.y >= camera_y && car_dto_world.y <= camera_y + viewport_height && (car_dto_world.state == IN_GAME || car_dto_world.state == NPC_STATE)) {
            
            CarDTO adjusted = car_dto_world;
            adjusted.x = (car_dto_world.x - camera_x) * ZOOM_FACTOR;
            adjusted.y = (car_dto_world.y - camera_y) * ZOOM_FACTOR;

            auto it = car_objects.find(id);
            if (it == car_objects.end()) {
                car_objects[id] = std::make_unique<Car>(adjusted.x, adjusted.y, adjusted.angle, renderer, resources.get(), car_dto_world.car_id, ZOOM_FACTOR);
                it = car_objects.find(id);
            }
            Car* car_obj = it->second.get();
            car_obj->update_from_dto(adjusted);
            car_obj->render();
        }
    }
}

GraphicClient::~GraphicClient() {
    upgrade_phase.reset();
    text.reset();
    resources.reset();
    
    if (TTF_WasInit()) {
        TTF_Quit();
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    IMG_Quit();
    SDL_Quit();
}
