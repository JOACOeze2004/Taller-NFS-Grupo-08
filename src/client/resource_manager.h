#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>

class ResourceManager {
public:
    explicit ResourceManager(SDL_Renderer* renderer);
    ~ResourceManager();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    SDL_Texture* load(const std::string& path);
    void unloadAll();

private:
    SDL_Renderer* renderer;
    std::unordered_map<std::string, SDL_Texture*> textures;
};

#endif // RESOURCE_MANAGER_H
