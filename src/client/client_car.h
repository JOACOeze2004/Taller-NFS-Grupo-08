#ifndef CAR_H
#define CAR_H

#include <SDL2/SDL.h>
#include <string>
#include "../common/car_DTO.h"
#include "resource_manager.h"
#include "sprite_loader.h"

class Car {
public:
    Car(float x, float y, float angle, SDL_Renderer* renderer, ResourceManager* resources, int car_id = 0, float scale = 1.0f);
    ~Car();

    void update_from_dto(const CarDTO& state);
    void render();

    float get_x() const { return x; }
    float get_y() const { return y; }
    float get_angle() const { return angle; }
    float get_velocity() const { return velocity; }

private:
    static constexpr float CAR_WIDTH = 20.0f;
    static constexpr float CAR_HEIGHT = 10.0f;
    
    static constexpr int LIFE_BAR_WIDTH = 50;
    static constexpr int LIFE_BAR_HEIGHT = 5;
    static constexpr int LIFE_BAR_OFFSET_X = 25;
    static constexpr int LIFE_BAR_OFFSET_Y = 40;
    
    static constexpr int NITRO_BAR_WIDTH = 50;
    static constexpr int NITRO_BAR_HEIGHT = 5;
    static constexpr int NITRO_BAR_OFFSET_X = 25;
    static constexpr int NITRO_BAR_OFFSET_Y = 35;
    
    static constexpr float NITRO_INDICATOR_DISTANCE = 20.0f;
    static constexpr int NITRO_INDICATOR_WIDTH = 30;
    static constexpr int NITRO_INDICATOR_HEIGHT = 25;
    static constexpr double NITRO_ROTATION_OFFSET = 180.0;
    
    static constexpr int NITRO_SPRITE_X = 30;
    static constexpr int NITRO_SPRITE_Y = 80;
    static constexpr int NITRO_SPRITE_WIDTH = 335;
    static constexpr int NITRO_SPRITE_HEIGHT = 97;
    
    static constexpr float NITRO_DIVISOR = 5.0f;
    static constexpr float LIFE_MAX_VALUE = 100.0f;
    static constexpr float NITRO_MAX_VALUE = 100.0f;
    
    static constexpr int DEFAULT_SPRITE_X = 2;
    static constexpr int DEFAULT_SPRITE_Y = 5;
    static constexpr int DEFAULT_SPRITE_WIDTH = 29;
    static constexpr int DEFAULT_SPRITE_HEIGHT = 22;
    
    static constexpr int MIN_CAR_ID = 0;
    static constexpr int MAX_CAR_ID = 6;
    
    static constexpr float DIRECTION_INDICATOR_LENGTH = 1.5f;

    float x;
    float y;
    float angle;
    float velocity;
    float nitro;
    float nitro_remaining;
    int life;
    float render_scale;
    
    SDL_Renderer* renderer;
    SDL_Texture* texture = nullptr;
    SDL_Texture* nitro_texture = nullptr;
    SDL_Rect srcRect{0, 0, 0, 0};
    
    void loadSpriteConfig(int car_id);
    void setDefaultSprite();
    
    void renderTexture();
    void renderFallback();
    void renderCarOutline();
    void renderDirectionIndicator();
    void renderNitro();
    void renderLife();
    void renderNitroBar();
    
    SDL_Rect createBarBackground(int offset_x, int offset_y, int width, int height) const;
    SDL_Rect createBarForeground(const SDL_Rect& background, float current, float max) const;
    SDL_Rect calculateNitroIndicatorRect() const;
};

#endif // CAR_H