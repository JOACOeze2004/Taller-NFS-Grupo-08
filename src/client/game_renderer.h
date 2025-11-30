#ifndef TALLER_GAME_RENDERER_H
#define TALLER_GAME_RENDERER_H

#include <SDL2/SDL.h>
#include <memory>
#include <unordered_map>
#include "client_car.h"
#include "resource_manager.h"
#include "../common/DTO.h"
#include "sprite_loader.h"

class GameRenderer {
private:
    SDL_Renderer* renderer;
    ResourceManager* resources;
    SDL_Texture* bg_texture;
    SDL_Texture* checkpoint_texture;
    SDL_Texture* hint_texture;
    SDL_Texture* speedometer_texture;
    
    std::unordered_map<int, std::unique_ptr<Car>> car_objects;
    std::vector<SpriteData> checkpoint_sprites;
    SpriteData hint_sprite;
    
    int screen_width;
    int screen_height;
    float map_width;
    float map_height;
    float zoom_factor;

public:
    GameRenderer(SDL_Renderer* rend, ResourceManager* res, int screen_w, int screen_h, float zoom);
    
    void load_map_texture(int map_id);
    void load_sprites_config(const std::string& yaml_path);
    
    void draw_background(float camera_x, float camera_y);
    void draw_cars(const std::unordered_map<int, CarDTO>& cars, float camera_x, float camera_y);
    void draw_checkpoint(const CheckpointCoords& checkpoint, int type, float camera_x, float camera_y);
    void draw_hint(const HintCoords& hint, float camera_x, float camera_y);
    void draw_speedometer(float speed);
    
    void update_car_objects(int, const CarDTO& car_state);
    void clear_car_objects(const std::unordered_map<int, CarDTO>& cars_in_dto);
    
    float get_map_width() const { return map_width; }
    float get_map_height() const { return map_height; }
    SDL_Texture* get_bg_texture() const { return bg_texture; }
    SDL_Texture* get_hint_texture() const { return hint_texture; }
};

#endif // TALLER_GAME_RENDERER_H