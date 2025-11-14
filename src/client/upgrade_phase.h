#ifndef TALLER_TP_UPGRADE_PHASE_H
#define TALLER_TP_UPGRADE_PHASE_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include "../common/DTO.h"
#include "text_renderer.h"
#include "client_handler.h"

class UpgradePhase {
private:
    SDL_Renderer* renderer;
    SDL_Window* window;
    int screen_width;
    int screen_height;
    TextRenderer* text;
    ClientHandler* handler;
    
    SDL_Texture* icons_texture;
    SDL_Texture* arrows_texture;
    
    Upgrades selected_upgrade;
    bool upgrade_selected;
    
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
    void load_textures();
    void render_background();
    void render_title();
    void render_upgrade_buttons();
    void render_instructions();

public:
    UpgradePhase(SDL_Renderer* renderer, SDL_Window* window, int screen_width, int screen_height, ClientHandler* _handler);
    ~UpgradePhase();
    
    Upgrades show_and_wait_selection();
    
    void render();
};

#endif  // TALLER_TP_UPGRADE_PHASE_H
