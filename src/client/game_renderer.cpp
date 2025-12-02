#include "game_renderer.h"
#include "config.h"
#include <SDL2/SDL_image.h>
#include <cmath>
#include <stdexcept>

GameRenderer::GameRenderer(SDL_Renderer* rend, ResourceManager* res, int screen_w, int screen_h, float zoom)
    : renderer(rend), resources(res), bg_texture(nullptr), checkpoint_texture(nullptr),
      hint_texture(nullptr), speedometer_texture(nullptr),
      screen_width(screen_w), screen_height(screen_h),
      map_width(0.0f), map_height(0.0f), zoom_factor(zoom) {
    
    checkpoint_texture = resources->load(CHECKPOINT_TEXTURE_PATH);
    hint_texture = resources->load(HINT_TEXTURE_PATH);
    speedometer_texture = resources->load(SPEEDOMETER_TEXTURE_PATH);
}

void GameRenderer::load_map_texture(int map_id) {
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
            throw std::runtime_error("[CLIENT] Invalid map ID: " + std::to_string(map_id));
    }
    
    SDL_Surface* bg_surface = IMG_Load(map_path.c_str());
    if (!bg_surface) {
        throw std::runtime_error(std::string("[CLIENT] Error loading background: ") + SDL_GetError());
    }
    
    map_width = static_cast<float>(bg_surface->w);
    map_height = static_cast<float>(bg_surface->h);
    SDL_FreeSurface(bg_surface);
    
    bg_texture = resources->load(map_path.c_str());
}

void GameRenderer::load_sprites_config(const std::string& yaml_path) {
    checkpoint_sprites = SpriteLoader::loadCheckpointSprites(yaml_path);
    hint_sprite = SpriteLoader::loadHintSprite(yaml_path);
}

void GameRenderer::draw_background(float camera_x, float camera_y) {
    if (!bg_texture) return;
    
    const float viewport_width = static_cast<float>(screen_width) / zoom_factor;
    const float viewport_height = static_cast<float>(screen_height) / zoom_factor;

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
        -fx * zoom_factor,
        -fy * zoom_factor,
        static_cast<float>(screen_width) + zoom_factor,
        static_cast<float>(screen_height) + zoom_factor
    };
    
    SDL_RenderCopyF(renderer, bg_texture, &src_rect, &dst_rect);
}

void GameRenderer::draw_cars(const std::unordered_map<int, CarDTO>& cars, float camera_x, float camera_y) {
    const float viewport_width = static_cast<float>(screen_width) / zoom_factor;
    const float viewport_height = static_cast<float>(screen_height) / zoom_factor;
  
    for (const auto& [id, car_dto_world] : cars) {
        if (car_dto_world.x >= camera_x && car_dto_world.x <= camera_x + viewport_width &&
            car_dto_world.y >= camera_y && car_dto_world.y <= camera_y + viewport_height && 
            (car_dto_world.state == IN_GAME || car_dto_world.state == NPC_STATE)) {
            
            CarDTO adjusted = car_dto_world;
            adjusted.x = (car_dto_world.x - camera_x) * zoom_factor;
            adjusted.y = (car_dto_world.y - camera_y) * zoom_factor;
            
            auto it = car_objects.find(id);
            if (it == car_objects.end()) {
                car_objects[id] = std::make_unique<Car>(adjusted.x, adjusted.y, adjusted.angle, 
                                                         renderer, resources, car_dto_world.car_id, zoom_factor);
                it = car_objects.find(id);
            }
            Car* car_obj = it->second.get();
            car_obj->update_from_dto(adjusted);
            car_obj->render();
        }
    }
}

void GameRenderer::draw_checkpoint(const CheckpointCoords& checkpoint, int type, float camera_x, float camera_y) {
    if (!checkpoint_texture) return;

    const float viewport_width = static_cast<float>(screen_width) / zoom_factor;
    const float viewport_height = static_cast<float>(screen_height) / zoom_factor;

    if (checkpoint.x >= camera_x && checkpoint.x <= camera_x + viewport_width &&
        checkpoint.y >= camera_y && checkpoint.y <= camera_y + viewport_height) {

        float screen_x = (checkpoint.x - camera_x) * zoom_factor;
        float screen_y = (checkpoint.y - camera_y) * zoom_factor;

        const float dst_w = CHECKPOINT_DST_W;
        const float dst_h = CHECKPOINT_DST_H;
        SDL_FRect dst_rect = {screen_x - dst_w * 0.5f, screen_y - dst_h * 0.5f, dst_w, dst_h};

        SDL_Rect src_rect = {290, 175, 95, 120};
        for (const auto& sprite : checkpoint_sprites) {
            if (sprite.type == type) {
                src_rect = sprite.rect;
                break;
            }
        }
        SDL_RenderCopyF(renderer, checkpoint_texture, &src_rect, &dst_rect);
    }
}

void GameRenderer::draw_hint(const HintCoords& hint, float camera_x, float camera_y) {
    if (!hint_texture) return;

    const float viewport_width = static_cast<float>(screen_width) / zoom_factor;
    const float viewport_height = static_cast<float>(screen_height) / zoom_factor;

    if (hint.x >= camera_x && hint.x <= camera_x + viewport_width &&
        hint.y >= camera_y && hint.y <= camera_y + viewport_height) {

        float screen_x = (hint.x - camera_x) * zoom_factor;
        float screen_y = (hint.y - camera_y) * zoom_factor;

        SDL_FRect dst_rect = {screen_x, screen_y, HINT_DST_W, HINT_DST_H};
        double angle_deg = hint.angle + 180.0;
        SDL_FPoint center = {15.0f, 15.0f};

        SDL_RenderCopyExF(renderer, hint_texture, &hint_sprite.rect,
                         &dst_rect, angle_deg, &center, SDL_FLIP_NONE);
    }
}

void GameRenderer::draw_speedometer(float speed) {
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

void GameRenderer::update_car_objects(int id, const CarDTO& car_state) {
    if (car_objects.find(id) == car_objects.end()) {
        car_objects[id] = std::make_unique<Car>(0.0f, 0.0f, 0.0f, renderer, resources, car_state.car_id, zoom_factor);
    }
}

void GameRenderer::clear_car_objects(const std::unordered_map<int, CarDTO>& cars_in_dto) {
    for (auto it = car_objects.begin(); it != car_objects.end(); ) {
        if (cars_in_dto.find(it->first) == cars_in_dto.end()) {
            it = car_objects.erase(it);
        } else {
            ++it;
        }
    }
}