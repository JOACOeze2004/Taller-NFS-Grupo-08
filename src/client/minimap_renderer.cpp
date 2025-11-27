#include "minimap_renderer.h"
#include <cmath>

MinimapRenderer::MinimapRenderer(SDL_Renderer* rend, int screen_w, int screen_h, float map_w, float map_h)
    : renderer(rend), bg_texture(nullptr), hint_texture(nullptr),
      screen_width(screen_w), screen_height(screen_h),
      map_width(map_w), map_height(map_h),
      minimap_scale(0.04f), player_car_id(-1) {}

void MinimapRenderer::set_textures(SDL_Texture* bg_tex, SDL_Texture* hint_tex) {
    bg_texture = bg_tex;
    hint_texture = hint_tex;
}

void MinimapRenderer::draw(const std::unordered_map<int, CarDTO>& cars, int camera_id,
                           const CheckpointCoords& checkpoint, int checkpoint_type, const HintCoords& hint) {
    if (!bg_texture) return;

    const int minimap_width = static_cast<int>(map_width * minimap_scale);
    const int minimap_height = static_cast<int>(map_height * minimap_scale);
    const int minimap_x = screen_width - minimap_width - 10;
    const int minimap_y = 10;

    const float radar_radius = 400.0f;
    
    if (camera_id < 0 || cars.find(camera_id) == cars.end()) {
        return;
    }
    
    const CarDTO& camera_car = cars.at(camera_id);
    
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

    // Draw cars
    for (const auto& [id, car] : cars) {
        if (car.x >= src_rect.x && car.x <= src_rect.x + src_rect.w &&
            car.y >= src_rect.y && car.y <= src_rect.y + src_rect.h && 
            (car.state == IN_GAME || car.state == NPC_STATE)) {
            
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