#ifndef TALLER_UI_RENDERER_H
#define TALLER_UI_RENDERER_H

#include <SDL2/SDL.h>
#include <memory>
#include <string>
#include <vector>
#include "text_renderer.h"
#include "client_handler.h"
#include "../common/DTO.h"
#include "sprite_loader.h"

class UIRenderer {
private:
    SDL_Renderer* renderer;
    std::unique_ptr<TextRenderer> text;
    ClientHandler* handler;
    int screen_width;
    int screen_height;
    static constexpr int HEADER_HEIGHT = 50;
    static constexpr int SETTINGS_BUTTON_SIZE = 30;
    static constexpr int SETTINGS_BUTTON_MARGIN = 10;

public:
    UIRenderer(SDL_Renderer* rend, const std::string& font_path, int font_size, 
               ClientHandler* hdlr, int screen_w, int screen_h);
    
    void draw_position(int position, int total_cars);
    void draw_time(int time_ms);
    void draw_game_id(int id);
    void draw_state(int state);
    void draw_ready_btn(int player_count, bool& ready_sent);
    void draw_results(const std::vector<CarRacingInfo>& cars_finished);
    void draw_checkpoints_info(int current_checkpoint, int total_checkpoints);
    void draw_upgrades_info(const std::map<Upgrades, int>& upgrades,
                           SDL_Texture* icons_texture,
                           const std::vector<UpgradeData>& upgrade_data);
    void draw_countdown(int time_ms);
    void draw_settings_button(bool is_hovered);
    SDL_Rect get_settings_button_rect() const;
    bool is_text_ok() const { return text && text->ok(); }
};

#endif // TALLER_UI_RENDERER_H