#ifndef TALLER_TP_GRAPHIC_CLIENT_H
#define TALLER_TP_GRAPHIC_CLIENT_H
#include <SDL2/SDL.h>
#include <string>
#include "client_car.h"
#include <unordered_map>
#include "../common/DTO.h"

class GraphicClient {
    SDL_Renderer* renderer;
    SDL_Texture* bg_texture;
    SDL_Window* window;
    std::unordered_map<int, CarDTO> cars; 

    int player_car_id;
    float camera_x, camera_y;
    int screen_width, screen_height;
    float map_width, map_height;
    

    public:
    explicit GraphicClient(const std::string& map_path, const DTO& initial_dto);
    void draw(const DTO& dto);
    void update_car(int id, const CarDTO& car_state);
    ~GraphicClient();
    
    private:
    void clear_cars(const std::unordered_map<int, CarDTO>& cars_in_dto);
    void draw_car(const CarDTO& car);
    void draw_cars();
    void draw_camera();
    void draw_minimap();
    void update_camera();
    void set_player_car(int id);
};


#endif  // TALLER_TP_GRAPHIC_CLIENT_H
