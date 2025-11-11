#ifndef CAR_H
#define CAR_H

#include <SDL2/SDL.h>
#include "../common/car_DTO.h"

class Car {
public:
    Car(float x, float y, float angle, SDL_Renderer* renderer, int car_id = 0, float scale = 1.0f);
    ~Car();

    void update_from_dto(const CarDTO& state);
    void render();

    float get_x() const { return x; }
    float get_y() const { return y; }
    float get_angle() const { return angle; }
    float get_velocity() const { return velocity; }

private:
    float x;
    float y;
    float angle;
    float velocity;
    float nitro;
    float nitro_remaining; // porcentaje 0..100 (o 0..1 si así viene)
    int life;
    float render_scale;  
    
    SDL_Renderer* renderer;
    SDL_Texture* texture = nullptr; 
    SDL_Texture* nitro_texture = nullptr;
    SDL_Rect srcRect{0, 0, 0, 0};   
    static constexpr float CAR_WIDTH = 20.0f;
    static constexpr float CAR_HEIGHT = 10.0f;
    void renderTexture();   
    void renderFallback();
    void renderCarOutline();
    void renderDirectionIndicator();
    void renderNitro();
    void renderLife();
    void renderNitroBar();
};

#endif // CAR_H