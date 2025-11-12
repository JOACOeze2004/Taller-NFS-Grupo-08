#ifndef TALLER_TP_UPGRADE_PHASE_H
#define TALLER_TP_UPGRADE_PHASE_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include "../common/DTO.h"
#include "text_renderer.h"

class UpgradePhase {
private:
    SDL_Renderer* renderer;
    SDL_Window* window;
    int screen_width;
    int screen_height;
    TextRenderer* text;
    
    Upgrades selected_upgrade;
    bool upgrade_selected;
    
    struct UpgradeButton {
        SDL_Rect rect;
        Upgrades upgrade_type;
        std::string title;
        std::string description;
        bool is_hovered;
    };
    
    std::vector<UpgradeButton> upgrade_buttons;
    
    void init_upgrade_buttons();
    void render_background();
    void render_title();
    void render_upgrade_buttons();
    void render_instructions();
    void handle_mouse_motion(int mouse_x, int mouse_y);
    void handle_mouse_click(int mouse_x, int mouse_y);
    bool is_point_in_rect(int x, int y, const SDL_Rect& rect);
    
public:
    UpgradePhase(SDL_Renderer* renderer, SDL_Window* window, int screen_width, int screen_height);
    ~UpgradePhase();
    
    Upgrades show_and_wait_selection();
    
    void render();
    
    void handle_event(const SDL_Event& event);
};

#endif  // TALLER_TP_UPGRADE_PHASE_H
