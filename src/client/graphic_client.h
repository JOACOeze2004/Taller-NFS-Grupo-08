#ifndef TALLER_TP_GRAPHIC_CLIENT_H
#define TALLER_TP_GRAPHIC_CLIENT_H
#include <SDL2/SDL.h>
#include <string>
#include "client_car.h"
#include <unordered_map>
#include <memory>
#include "../common/DTO.h"
#include "text_renderer.h"

class GraphicClient {
    SDL_Renderer* renderer;
    SDL_Texture* bg_texture;
    SDL_Texture* checkpoint_texture;
    SDL_Texture* hint_texture;
    SDL_Window* window;
    std::unordered_map<int, CarDTO> cars;
    std::unordered_map<int, std::unique_ptr<Car>> car_objects;

    int player_car_id;
    float camera_x, camera_y;
    int screen_width, screen_height;
    float map_width, map_height;
    TextRenderer* text;

    

    public:
    explicit GraphicClient(const std::string& map_path, const Snapshot& initial_snapshot);
    void draw(const Snapshot& snapshot);
    /* void update_from_snapshot(snapshot); */
    void update_car(int id, const CarDTO& car_state); //este ya no se usa
    ~GraphicClient();
    
    private:
    void clear_cars(const std::unordered_map<int, CarDTO>& cars_in_dto);
    void draw_car(const CarDTO& car, int car_id);
    void draw_cars();
    void draw_camera();
    void draw_minimap(const CheckpointCoords& checkpoint, int checkpoint_type, const HintCoords& hint);
    void update_camera();
    void set_player_car(int id);
    void draw_position(int position, int total_cars);
    void draw_time(int time_ms);
    void draw_speed(float speed);
    void draw_life(int life);
    void draw_nitro(bool nitro);
    void draw_checkpoint(CheckpointCoords checkpoint, int type);
    void draw_hint(HintCoords hint);
    void draw_id_partida(int id);
};


#endif  // TALLER_TP_GRAPHIC_CLIENT_H
