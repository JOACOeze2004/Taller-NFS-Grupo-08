#ifndef TALLER_TP_UPGRADE_PHASE_H
#define TALLER_TP_UPGRADE_PHASE_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <memory>
#include "../common/DTO.h"
#include "text_renderer.h"
#include "client_handler.h"

constexpr int UPGRADE_FONT_SIZE = 18;
constexpr const char* UPGRADE_FONT_PATH = "../assets/fonts/DejaVuSans.ttf";
constexpr const char* WORKSHOP_ICONS_PATH = "../assets/need-for-speed/sprits/Workshop_icons_1.png";
constexpr const char* WORKSHOP_ARROWS_PATH = "../assets/need-for-speed/sprits/workshop_arrow_spritesheet.png";

class UpgradePhase {
private:
    SDL_Renderer* renderer;
    SDL_Window* window;
    int screen_width;
    int screen_height;
    std::unique_ptr<TextRenderer> text;
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
    void addUpgrade(int order, const char* title, const char* desc, SDL_Rect icon_src,
                    ButtonType left_button_type, ButtonType right_button_type,
                    int start_x, int start_y, int button_width, int button_height,
                    int spacing, int icon_size, int arrow_size);
    void load_textures();
    void render_background();
    void render_title();
    void render_upgrade_buttons();
    void render_instructions();
    void render_remaining_upgrades(int remaining_upgrades);

public:
    UpgradePhase(SDL_Renderer* renderer, SDL_Window* window, int screen_width, int screen_height, ClientHandler* _handler);
    ~UpgradePhase();
    
    Upgrades show_and_wait_selection();
    
    void render(int remaining_upgrades);
};

#endif  // TALLER_TP_UPGRADE_PHASE_H
