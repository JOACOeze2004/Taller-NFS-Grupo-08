#include "client_car.h"
#include <cmath>
#include <string>
#include <vector>
#include <SDL2/SDL_image.h>

namespace {
SDL_Texture* loadTextureFromCandidates(SDL_Renderer* renderer, const std::vector<std::string>& candidates) {
    for (const auto& path : candidates) {
        SDL_Texture* tex = IMG_LoadTexture(renderer, path.c_str());
        if (tex) return tex;
    }
    return nullptr;
}
}

Car::Car(float x, float y, float angle, SDL_Renderer* renderer, float scale)
    : x(x), y(y), angle(angle), velocity(0.0f), render_scale(scale), renderer(renderer) {
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
    }

    const std::vector<std::string> candidates = {
        "../assets/need-for-speed/cars/Cars_without_bg.png",
        "assets/need-for-speed/cars/Cars_without_bg.png",
    };

    texture = loadTextureFromCandidates(renderer, candidates);
    //esto se encarga de agarrar la imagen del coche, ver como manejarlo cuando haya q cambiar los coches
    //es segun el id del coche leer el id del yaml y cargar otro sprite
    if (texture) {
        srcRect.x = 2;
        srcRect.y = 5;
        srcRect.w = 29;
        srcRect.h = 22;
    } else {
        srcRect = {0, 0, 0, 0};
    }
}

Car::~Car() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

void Car::update_from_dto(const CarDTO& state) {
    x = state.x;
    y = state.y;
    angle = state.angle;
    velocity = state.velocity;
    /* agregar vida y alguna otra informacion como tiene nitro esta abajo de puente etc */
}

void Car::render() {
    if (texture && srcRect.w > 0 && srcRect.h > 0) {
        SDL_Rect dstRect;
        dstRect.w = static_cast<int>(CAR_WIDTH * render_scale);
        dstRect.h = static_cast<int>(CAR_HEIGHT * render_scale);
        dstRect.x = static_cast<int>(x - dstRect.w / 2.0f);
        dstRect.y = static_cast<int>(y - dstRect.h / 2.0f);

        const double angleDeg = angle * 180.0 / M_PI;
        SDL_Point center{dstRect.w / 2, dstRect.h / 2};

        SDL_RenderCopyEx(renderer, texture, &srcRect, &dstRect, angleDeg, &center, SDL_FLIP_NONE);
        return;
    }

    float half_width = (CAR_WIDTH * render_scale) / 2.0f;
    float half_height = (CAR_HEIGHT * render_scale) / 2.0f;

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

    //modularizar y agregar dibujar nitro y dibujar vida
}