#ifndef TALLER_TP_GRAPHIC_CLIENT_H
#define TALLER_TP_GRAPHIC_CLIENT_H
#include <SDL2/SDL.h>
#include <string>
#include "client_car.h"
#include <unordered_map>

class GraphicClient {
    SDL_Renderer* renderer;
    SDL_Texture* bg_texture;
    SDL_Window* window;
    std::unordered_map<int, CarDTO> cars; 

    public:
    explicit GraphicClient(const std::string& map_path);
    void draw();
    void update_car(int id, const CarDTO& car_state);
    ~GraphicClient();

    private:
        void draw_car(const CarDTO& car);
};


#endif  // TALLER_TP_GRAPHIC_CLIENT_H
