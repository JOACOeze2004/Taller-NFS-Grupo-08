#ifndef TALLER_MINIMAP_RENDERER_H
#define TALLER_MINIMAP_RENDERER_H

#include <SDL2/SDL.h>
#include <unordered_map>
#include "../common/DTO.h"

class MinimapRenderer {
private:
    SDL_Renderer* renderer;
    SDL_Texture* bg_texture;
    SDL_Texture* hint_texture;
    
    int screen_width;
    int screen_height;
    float map_width;
    float map_height;
    float minimap_scale;
    int player_car_id;

public:
    MinimapRenderer(SDL_Renderer* rend, int screen_w, int screen_h, float map_w, float map_h);
    
    void set_textures(SDL_Texture* bg_tex, SDL_Texture* hint_tex);
    void set_player_car_id(int id) { player_car_id = id; }
    
    void draw(const std::unordered_map<int, CarDTO>& cars, int camera_id,
              const CheckpointCoords& checkpoint, int checkpoint_type, const HintCoords& hint);
};

#endif // TALLER_MINIMAP_RENDERER_H