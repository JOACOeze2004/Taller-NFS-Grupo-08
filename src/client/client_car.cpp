#include "client_car.h"
#include <cmath>

Car::Car(float x, float y, SDL_Renderer* renderer)
    : x(x), y(y), angle(0.0f), velocity(0.0f), renderer(renderer) {}

void Car::update_from_dto(const CarDTO& state) {
    x = state.x +200;
    y = state.y +200;
    angle = state.angle;
    velocity = state.velocity;
}

void Car::render() {
    // Convertir ángulo a radianes
    float angle_rad = angle;
    
    // Calcular las 4 esquinas del rectángulo rotado
    float half_width = CAR_WIDTH / 2.0f;
    float half_height = CAR_HEIGHT / 2.0f;
    
    SDL_Point corners[5]; // 5 puntos para cerrar el polígono
    
    // Esquinas sin rotar (relativas al centro)
    float local_corners[4][2] = {
        {-half_width, -half_height}, // Top-left
        { half_width, -half_height}, // Top-right
        { half_width,  half_height}, // Bottom-right
        {-half_width,  half_height}  // Bottom-left
    };
    
    // Aplicar rotación y traslación
    for (int i = 0; i < 4; i++) {
        float lx = local_corners[i][0];
        float ly = local_corners[i][1];
        
        // Rotar
        float rotated_x = lx * cos(angle_rad) - ly * sin(angle_rad);
        float rotated_y = lx * sin(angle_rad) + ly * cos(angle_rad);
        
        // Trasladar
        corners[i].x = static_cast<int>(x + rotated_x);
        corners[i].y = static_cast<int>(y + rotated_y);
    }
    
    // Cerrar el polígono
    corners[4] = corners[0];
    
    // Dibujar el auto (rectángulo azul)
    SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);
    
    // Dibujar las líneas del rectángulo
    for (int i = 0; i < 4; i++) {
        SDL_RenderDrawLine(renderer, 
                          corners[i].x, corners[i].y,
                          corners[i+1].x, corners[i+1].y);
    }
    
    // Dibujar una línea al frente para indicar dirección
    float front_x = x + half_width * 1.5f * cos(angle_rad);
    float front_y = y + half_width * 1.5f * sin(angle_rad);
    
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer, 
                      static_cast<int>(x), static_cast<int>(y),
                      static_cast<int>(front_x), static_cast<int>(front_y));
}