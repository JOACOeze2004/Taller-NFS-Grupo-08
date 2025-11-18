#include "client_car.h"
#include <cmath>
#include <string>
#include <SDL2/SDL_image.h>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <fstream>

namespace {
    const std::string CAR_TEXTURE_PATH = "../assets/need-for-speed/cars/Cars_without_bg.png";
    const std::string YAML_CONFIG_PATH = "../src/client/car_sprites.yaml";
    const std::string NITRO_TEXTURE_PATH = "../assets/need-for-speed/sprits/nitro-fire.png";
}

Car::Car(float x, float y, float angle, SDL_Renderer* renderer, int car_id, float scale)
    : x(x), y(y), angle(angle), velocity(0.0f), nitro(0.0f), nitro_remaining(0.0f),
      life(100), render_scale(scale), renderer(renderer) {
    
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
        std::cerr << "Warning: Failed to initialize SDL_Image PNG support: " << IMG_GetError() << std::endl;
    }
    
    loadTextures(car_id);
}

Car::~Car() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    if (nitro_texture) {
        SDL_DestroyTexture(nitro_texture);
        nitro_texture = nullptr;
    }
}

void Car::loadTextures(int car_id) {
    loadCarTexture();
    loadNitroTexture();
    loadSpriteConfig(car_id);
}

void Car::loadCarTexture() {
    texture = IMG_LoadTexture(renderer, CAR_TEXTURE_PATH.c_str());
    if (!texture) {
        std::cerr << "Warning: Could not load car texture from " << CAR_TEXTURE_PATH 
                  << ": " << IMG_GetError() << std::endl;
    }
}

void Car::loadNitroTexture() {
    nitro_texture = IMG_LoadTexture(renderer, NITRO_TEXTURE_PATH.c_str());
    if (!nitro_texture) {
        std::cerr << "Warning: Could not load nitro texture from " << NITRO_TEXTURE_PATH 
                  << ": " << IMG_GetError() << std::endl;
    }
}

void Car::loadSpriteConfig(int car_id) {
    if (!texture) {
        return;
    }
    
    if (car_id < MIN_CAR_ID || car_id > MAX_CAR_ID) {
        std::cerr << "Warning: car_id " << car_id << " out of range [" 
                  << MIN_CAR_ID << ", " << MAX_CAR_ID << "]. Using default." << std::endl;
        car_id = MIN_CAR_ID;
    }
    
    if (!tryLoadYamlConfig(YAML_CONFIG_PATH, car_id)) {
        setDefaultSprite();
    }
}

bool Car::tryLoadYamlConfig(const std::string& yaml_path, int car_id) {
    std::ifstream file(yaml_path);
    if (!file.good()) {
        return false;
    }
    
    try {
        YAML::Node config = YAML::LoadFile(yaml_path);
        
        if (!config["cars"] || !config["cars"].IsSequence()) {
            return false;
        }
        
        if (static_cast<std::size_t>(car_id) >= config["cars"].size()) {
            return false;
        }
        
        YAML::Node carSprite = config["cars"][car_id]["sprite"];
        srcRect.x = carSprite["x"].as<int>();
        srcRect.y = carSprite["y"].as<int>();
        srcRect.w = carSprite["width"].as<int>();
        srcRect.h = carSprite["height"].as<int>();
        
        return true;
        
    } catch (const YAML::Exception& e) {
        std::cerr << "Error parsing YAML file " << yaml_path << ": " << e.what() << std::endl;
        return false;
    }
}

void Car::setDefaultSprite() {
    srcRect.x = DEFAULT_SPRITE_X;
    srcRect.y = DEFAULT_SPRITE_Y;
    srcRect.w = DEFAULT_SPRITE_WIDTH;
    srcRect.h = DEFAULT_SPRITE_HEIGHT;
}

void Car::update_from_dto(const CarDTO& state) {
    x = state.x;
    y = state.y;
    angle = state.angle;
    velocity = state.velocity;
    life = state.life;
    nitro = state.nitro;
    nitro_remaining = state.remaining_nitro;
}

void Car::render() {
    if (texture && srcRect.w > 0 && srcRect.h > 0) {
        renderNitro();
        renderTexture();
        renderLife();
        renderNitroBar();
    } else {
        renderFallback();
    }
}

void Car::renderTexture() {
    SDL_Rect dstRect;
    dstRect.w = static_cast<int>(CAR_WIDTH * render_scale);
    dstRect.h = static_cast<int>(CAR_HEIGHT * render_scale);
    dstRect.x = static_cast<int>(x - dstRect.w / 2.0f);
    dstRect.y = static_cast<int>(y - dstRect.h / 2.0f);

    const double angleDeg = angle * 180.0 / M_PI;
    const SDL_Point center{dstRect.w / 2, dstRect.h / 2};

    SDL_RenderCopyEx(renderer, texture, &srcRect, &dstRect, angleDeg, &center, SDL_FLIP_NONE);
}

void Car::renderFallback() {
    renderCarOutline();
    renderDirectionIndicator();
}

void Car::renderCarOutline() {
    const float half_width = (CAR_WIDTH * render_scale) / 2.0f;
    const float half_height = (CAR_HEIGHT * render_scale) / 2.0f;

    SDL_Point corners[5];
    const float local_corners[4][2] = {
        {-half_width, -half_height},
        { half_width, -half_height},
        { half_width,  half_height},
        {-half_width,  half_height}
    };

    for (int i = 0; i < 4; i++) {
        const float lx = local_corners[i][0];
        const float ly = local_corners[i][1];
        const float rotated_x = lx * std::cos(angle) - ly * std::sin(angle);
        const float rotated_y = lx * std::sin(angle) + ly * std::cos(angle);
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
    const float half_width = (CAR_WIDTH * render_scale) / 2.0f;
    const float front_x = x + half_width * DIRECTION_INDICATOR_LENGTH * std::cos(angle);
    const float front_y = y + half_width * DIRECTION_INDICATOR_LENGTH * std::sin(angle);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer, static_cast<int>(x), static_cast<int>(y),
                      static_cast<int>(front_x), static_cast<int>(front_y));
}

SDL_Rect Car::calculateNitroIndicatorRect() const {
    const float car_length = (CAR_HEIGHT * render_scale) / 2.0f;
    const float indicator_distance = car_length + NITRO_INDICATOR_DISTANCE;
    
    SDL_Rect nitroIndicator;
    nitroIndicator.w = NITRO_INDICATOR_WIDTH;
    nitroIndicator.h = NITRO_INDICATOR_HEIGHT;
    nitroIndicator.x = static_cast<int>(x - indicator_distance * std::cos(angle) - nitroIndicator.w / 2.0f);
    nitroIndicator.y = static_cast<int>(y - indicator_distance * std::sin(angle) - nitroIndicator.h / 2.0f);
    
    return nitroIndicator;
}

void Car::renderNitro() {
    if (!nitro) {
        return;
    }
    
    const SDL_Rect nitroIndicator = calculateNitroIndicatorRect();
    
    if (nitro_texture) {
        const SDL_Rect nitroSrcRect = {
            NITRO_SPRITE_X, 
            NITRO_SPRITE_Y, 
            NITRO_SPRITE_WIDTH, 
            NITRO_SPRITE_HEIGHT
        };
        
        const double angleDeg = angle * 180.0 / M_PI + NITRO_ROTATION_OFFSET;
        const SDL_Point center{nitroIndicator.w / 2, nitroIndicator.h / 2};
        
        SDL_RenderCopyEx(renderer, nitro_texture, &nitroSrcRect, &nitroIndicator, 
                        angleDeg, &center, SDL_FLIP_NONE);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &nitroIndicator);
    }
}

SDL_Rect Car::createBarBackground(int offset_x, int offset_y, int width, int height) const {
    SDL_Rect bar;
    bar.x = static_cast<int>(x - offset_x);
    bar.y = static_cast<int>(y - offset_y);
    bar.w = width;
    bar.h = height;
    return bar;
}

SDL_Rect Car::createBarForeground(const SDL_Rect& background, float current, float max) const {
    SDL_Rect bar;
    bar.x = background.x;
    bar.y = background.y;
    bar.w = static_cast<int>(background.w * current / max);
    bar.h = background.h;
    return bar;
}

void Car::renderLife() {
    const SDL_Rect lifeBarBg = createBarBackground(
        LIFE_BAR_OFFSET_X, 
        LIFE_BAR_OFFSET_Y, 
        LIFE_BAR_WIDTH, 
        LIFE_BAR_HEIGHT
    );
    
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &lifeBarBg);
    
    const SDL_Rect lifeBarFg = createBarForeground(lifeBarBg, static_cast<float>(life), LIFE_MAX_VALUE);
    
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &lifeBarFg);
}

void Car::renderNitroBar() {
    const float nitro_percentage = nitro_remaining / NITRO_DIVISOR;
    
    const SDL_Rect nitroBg = createBarBackground(
        NITRO_BAR_OFFSET_X, 
        NITRO_BAR_OFFSET_Y, 
        NITRO_BAR_WIDTH, 
        NITRO_BAR_HEIGHT
    );
    
    SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
    SDL_RenderFillRect(renderer, &nitroBg);
    
    const SDL_Rect nitroFg = createBarForeground(nitroBg, nitro_percentage, NITRO_MAX_VALUE);
    
    SDL_SetRenderDrawColor(renderer, 0, 150, 255, 255);
    SDL_RenderFillRect(renderer, &nitroFg);
}