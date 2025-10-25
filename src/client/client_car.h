#ifndef CAR_H
#define CAR_H

#include <SDL2/SDL.h>
#include "../common/car_DTO.h"

class Car {
public:
    Car(float x, float y, SDL_Renderer* renderer);
    
    // Actualizar estado desde el servidor
    void update_from_dto(const CarDTO& state);
    
    // Renderizar el auto
    void render();
    
    // Getters
    float get_x() const { return x; }
    float get_y() const { return y; }
    float get_angle() const { return angle; }
    float get_velocity() const { return velocity; }

private:
    float x;
    float y;
    float angle;
    float velocity;
    
    SDL_Renderer* renderer;
    
    // Dimensiones del auto
    static constexpr float CAR_WIDTH = 20.0f;
    static constexpr float CAR_HEIGHT = 10.0f;
};

#endif // CAR_H