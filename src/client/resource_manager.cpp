#include "resource_manager.h"
#include <SDL2/SDL_image.h>
#include <iostream>

ResourceManager::ResourceManager(SDL_Renderer* renderer) 
    : renderer(renderer) {}

ResourceManager::~ResourceManager() {
    unloadAll();
}

SDL_Texture* ResourceManager::load(const std::string& path) {
    auto it = textures.find(path);
    if (it != textures.end()) {
        return it->second;
    }
    
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "[RESOURCES] Failed to load " << path 
                  << ": " << IMG_GetError() << std::endl;
        return nullptr;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!texture) {
        std::cerr << "[RESOURCES] Failed to create texture from " << path 
                  << ": " << SDL_GetError() << std::endl;
        return nullptr;
    }
    
    textures[path] = texture;
    return texture;
}

void ResourceManager::unloadAll() {
    for (auto& [path, texture] : textures) {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }
    textures.clear();
}
