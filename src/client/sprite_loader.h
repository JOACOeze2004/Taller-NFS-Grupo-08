#ifndef TALLER_SPRITE_LOADER_H
#define TALLER_SPRITE_LOADER_H

#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <yaml-cpp/yaml.h>

struct SpriteData {
    int id;
    std::string name;
    SDL_Rect rect;
    int type;
};

struct UpgradeData {
    int id;
    std::string name;
    std::string title;
    std::string description;
    int order;
    SDL_Rect sprite;
    std::string button_left;
    std::string button_right;
};

struct ArrowData {
    SDL_Rect sprite;
};

class SpriteLoader {
public:
    static std::vector<SpriteData> loadCarSprites(const std::string& yaml_path);
    static std::vector<SpriteData> loadCheckpointSprites(const std::string& yaml_path);
    static SpriteData loadHintSprite(const std::string& yaml_path);
    static std::vector<UpgradeData> loadUpgradeSprites(const std::string& yaml_path);
    static ArrowData loadArrowSprite(const std::string& yaml_path);

private:
    static bool fileExists(const std::string& path);
};

#endif // TALLER_SPRITE_LOADER_H