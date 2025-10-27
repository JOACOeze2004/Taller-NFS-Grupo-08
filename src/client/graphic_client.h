#ifndef TALLER_TP_GRAPHIC_CLIENT_H
#define TALLER_TP_GRAPHIC_CLIENT_H
#include <SDL2/SDL.h>

#include "client_car.h"

class GraphicClient {
    SDL_Renderer* renderer;
    SDL_Texture* bg_texture;
    SDL_Window* window;
    Car car;
public:
    explicit GraphicClient();
    void draw();
    void update_car(const CarDTO& state);
    ~GraphicClient();

};


#endif  // TALLER_TP_GRAPHIC_CLIENT_H
