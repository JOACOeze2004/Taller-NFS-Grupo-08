#ifndef TALLER_TP_UPGRADE_PHASE_H
#define TALLER_TP_UPGRADE_PHASE_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <memory>
#include "../common/DTO.h"
#include "text_renderer.h"
#include "client_handler.h"
#include "resource_manager.h"
#include "config.h"
#include "sprite_loader.h"

class UpgradePhase {
private:
    SDL_Renderer* renderer;
    SDL_Window* window;
    int screen_width;
    int screen_height;
    std::unique_ptr<TextRenderer> text;
    ClientHandler* handler;
    ResourceManager* resources;
    
    SDL_Texture* icons_texture;
    SDL_Texture* arrows_texture;
    
    Upgrades selected_upgrade;
    bool upgrade_selected;

    std::vector<UpgradeData> upgrade_data;
    ArrowData arrow_data;
    
    struct UpgradeButton {
        SDL_Rect rect; 
        Upgrades upgrade_type;
        std::string title;
        std::string description;
        SDL_Rect icon_src_rect;
    };

    struct ArrowButton {
        SDL_Rect rect;              
        ButtonType type;            
    };
    
    std::vector<UpgradeButton> upgrade_buttons;
    std::vector<ArrowButton> arrow_buttons;
    
    void init_upgrade_buttons();
    void load_upgrade_sprites();
    ButtonType getButtonType(const std::string& button_name);
    void load_textures();
    void render_background();
    void render_title();
    void render_upgrade_buttons();
    void render_instructions();
    void render_remaining_upgrades(int remaining_upgrades);

public:
    UpgradePhase(SDL_Renderer* renderer, SDL_Window* window, int screen_width, int screen_height, 
                 ClientHandler* _handler, ResourceManager* res);
    ~UpgradePhase();
    
    Upgrades show_and_wait_selection();
    
    void render(int remaining_upgrades);
};

#endif  // TALLER_TP_UPGRADE_PHASE_H
