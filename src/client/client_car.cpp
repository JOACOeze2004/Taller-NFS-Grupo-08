#include "client_car.h"
#include <cmath>

Car::Car(float x, float y, float angle, SDL_Renderer* renderer)
    : x(x), y(y), angle(angle), velocity(0.0f), renderer(renderer) {}

void Car::update_from_dto(const CarDTO& state) {
    x = state.x;
    y = state.y;
    angle = state.angle;
    velocity = state.velocity;
}

void Car::render() {
    float half_width = CAR_WIDTH / 2.0f;
    float half_height = CAR_HEIGHT / 2.0f;
    
    SDL_Point corners[5];

    float local_corners[4][2] = {
        {-half_width, -half_height},
        { half_width, -half_height},
        { half_width,  half_height},
        {-half_width,  half_height}
    };

    for (int i = 0; i < 4; i++) {
        float lx = local_corners[i][0];
        float ly = local_corners[i][1];

        float rotated_x = lx * cos(angle) - ly * sin(angle);
        float rotated_y = lx * sin(angle) + ly * cos(angle);

        corners[i].x = static_cast<int>(x + rotated_x);
        corners[i].y = static_cast<int>(y + rotated_y);
    }

    corners[4] = corners[0];

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

    for (int i = 0; i < 4; i++) {
        SDL_RenderDrawLine(renderer, 
                          corners[i].x, corners[i].y,
                          corners[i+1].x, corners[i+1].y);
    }

    float front_x = x + half_width * 1.5f * cos(angle);
    float front_y = y + half_width * 1.5f * sin(angle);
    
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer, 
                      static_cast<int>(x), static_cast<int>(y),
                      static_cast<int>(front_x), static_cast<int>(front_y));
}