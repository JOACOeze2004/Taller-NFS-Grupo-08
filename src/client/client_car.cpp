#include "client_car.h"
#include <cmath>
#include <string>
#include <vector>
#include <SDL2/SDL_image.h>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <fstream>

namespace {
SDL_Texture* loadTextureFromCandidates(SDL_Renderer* renderer, const std::vector<std::string>& candidates) {
    for (const auto& path : candidates) {
        SDL_Texture* tex = IMG_LoadTexture(renderer, path.c_str());
        if (tex) return tex;
    }
    return nullptr;
}
}

Car::Car(float x, float y, float angle, SDL_Renderer* renderer, int car_id, float scale)
    : x(x), y(y), angle(angle), velocity(0.0f), render_scale(scale), renderer(renderer) {
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
    }

    const std::vector<std::string> candidates = {
        "../assets/need-for-speed/cars/Cars_without_bg.png",
        "assets/need-for-speed/cars/Cars_without_bg.png",
    };

    texture = loadTextureFromCandidates(renderer, candidates);
    
    if (texture) {
        const std::vector<std::string> yamlCandidates = {
            "../src/client/car_sprites.yaml",
            "src/client/car_sprites.yaml",
        };
        
        bool yamlLoaded = false;
        for (const auto& yamlPath : yamlCandidates) {
            std::ifstream file(yamlPath);
            if (file.good()) {
                try {
                    YAML::Node config = YAML::LoadFile(yamlPath);
                    
                    if (car_id < 0 || car_id > 6) {
                        std::cerr << "Warning: car_id " << car_id << " out of range. Using car 0." << std::endl;
                        car_id = 0;
                    }
                    
                    if (config["cars"] && config["cars"].IsSequence() && static_cast<std::size_t>(car_id) < config["cars"].size()) {
                        YAML::Node carSprite = config["cars"][car_id]["sprite"];
                        srcRect.x = carSprite["x"].as<int>();
                        srcRect.y = carSprite["y"].as<int>();
                        srcRect.w = carSprite["width"].as<int>();
                        srcRect.h = carSprite["height"].as<int>();
                        yamlLoaded = true;
                        break;
                    }

                } catch (const YAML::Exception& e) {
                    std::cerr << "Error parsing YAML file " << yamlPath << ": " << e.what() << std::endl;
                }
            }
        }
        //si no agarre del yaml uso el verde defa
        if (!yamlLoaded) {
            srcRect.x = 2;
            srcRect.y = 5;
            srcRect.w = 29;
            srcRect.h = 22;
        }
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
    life = state.life;
    nitro = state.nitro;
    /* agregar vida y alguna otra informacion como tiene nitro esta abajo de puente etc */

}

void Car::render() {
    if (texture && srcRect.w > 0 && srcRect.h > 0) {
        renderTexture();
    } else {
        renderFallback();
    }
    // Render nitro y vida (debug temporal)
    std::cout << "Render Nitro " << nitro << std::endl;
    std::cout << "Render Life " << life << std::endl;
}

void Car::renderTexture() {
    SDL_Rect dstRect;
    dstRect.w = static_cast<int>(CAR_WIDTH * render_scale);
    dstRect.h = static_cast<int>(CAR_HEIGHT * render_scale);
    dstRect.x = static_cast<int>(x - dstRect.w / 2.0f);
    dstRect.y = static_cast<int>(y - dstRect.h / 2.0f);

    const double angleDeg = angle * 180.0 / M_PI;
    SDL_Point center{dstRect.w / 2, dstRect.h / 2};

    SDL_RenderCopyEx(renderer, texture, &srcRect, &dstRect, angleDeg, &center, SDL_FLIP_NONE);
}

void Car::renderFallback() {
    renderCarOutline();
    renderDirectionIndicator();
}

void Car::renderCarOutline() {
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
        SDL_RenderDrawLine(renderer, corners[i].x, corners[i].y, corners[i+1].x, corners[i+1].y);
    }
}

void Car::renderDirectionIndicator() {
    float half_width = (CAR_WIDTH * render_scale) / 2.0f;
    float front_x = x + half_width * 1.5f * cos(angle);
    float front_y = y + half_width * 1.5f * sin(angle);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer, static_cast<int>(x), static_cast<int>(y),
                      static_cast<int>(front_x), static_cast<int>(front_y));
}

void Car::renderNitro() {
    // TODO: Implement nitro visual indicator
}

void Car::renderLife() {
    // TODO: Implement life bar
}