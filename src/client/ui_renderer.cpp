#include "ui_renderer.h"
#include <cstdio>

UIRenderer::UIRenderer(SDL_Renderer* rend, const std::string& font_path, int font_size,
                       ClientHandler* hdlr, int screen_w, int screen_h)
    : renderer(rend), handler(hdlr), screen_width(screen_w), screen_height(screen_h) {
    text = std::make_unique<TextRenderer>(font_path, font_size);
}

void UIRenderer::draw_text_with_outline(const std::string& msg, int x, int y) {
    if (!text) return;
    
    SDL_Color black{0, 0, 0, 255};
    text->render(renderer, msg, x - 2, y, black);
    text->render(renderer, msg, x + 2, y, black);
    text->render(renderer, msg, x, y - 2, black);
    text->render(renderer, msg, x, y + 2, black);
    
    SDL_Color white{255, 255, 255, 255};
    text->render(renderer, msg, x, y, white);
}

void UIRenderer::draw_position(int position, int total_cars) {
    if (!text) return;
    std::string msg = "POSITION: " + std::to_string(position) + " / " + std::to_string(total_cars);
    draw_text_with_outline(msg, 500, 30);
}

void UIRenderer::draw_time(int time_ms) {
    if (!text) return;
    int total_seconds = time_ms / 1000;
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;
    int milliseconds = time_ms % 1000;
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "TIME: %02d:%02d.%03d", minutes, seconds, milliseconds);
    draw_text_with_outline(buffer, 500, 100);
}

void UIRenderer::draw_game_id(int id) {
    if (!text) return;
    std::string msg = "GAME ID: " + std::to_string(id);
    draw_text_with_outline(msg, screen_width - 150, 200);
}

void UIRenderer::draw_state(int state) {
    if (!text) return;
    std::string msg;
    SDL_Color overlay_color;
    
    if (state == FINISHED) {
        msg = "You Finished!";
        overlay_color = {0, 255, 0, 76};
    } else if (state == DEAD) {
        msg = "You Died!";
        overlay_color = {255, 0, 0, 76};
    } else if (state == TIMEOUTED) {
        msg = "Time Out!";
        overlay_color = {255, 0, 0, 76};
    } else {
        return;
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, overlay_color.r, overlay_color.g, overlay_color.b, overlay_color.a);
    SDL_Rect fullscreen_rect = {0, 0, screen_width, screen_height};
    SDL_RenderFillRect(renderer, &fullscreen_rect);
    
    SDL_Color text_color{255, 255, 255, 255};
    int text_x = (screen_width / 2) - 100;
    int text_y = (screen_height / 2) - 25;
    text->render(renderer, msg, text_x, text_y, text_color);
}

void UIRenderer::draw_ready_btn(int player_count, bool& ready_sent) {
    if (!text || !handler) return;

    const int btn_w = 250;
    const int btn_h = 50;
    const int btn_x = (screen_width / 2) - (btn_w / 2);
    const int btn_y = screen_height - 120;
    SDL_Rect btn_rect = {btn_x, btn_y, btn_w, btn_h};

    if (!ready_sent) {
        handler->clear_buttons();
        handler->register_button(btn_rect, BUTTON_READY);
    } else {
        handler->clear_buttons();
    }

    bool is_hovered = handler->is_mouse_over_button(btn_rect);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if (ready_sent) {
        SDL_SetRenderDrawColor(renderer, 0, 180, 0, 200);
    } else if (is_hovered) {
        SDL_SetRenderDrawColor(renderer, 50, 164, 255, 230);
    } else {
        SDL_SetRenderDrawColor(renderer, 30, 144, 255, 200);
    }
    SDL_RenderFillRect(renderer, &btn_rect);

    if (is_hovered && !ready_sent) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
    }
    SDL_RenderDrawRect(renderer, &btn_rect);

    SDL_Color label_color{255, 255, 255, 255};
    std::string label = ready_sent ? "READY (sent)" : "READY";
    text->render(renderer, label, btn_x + btn_w / 2 - 40, btn_y + btn_h / 2 - 10, label_color);

    char buf[64];
    std::snprintf(buf, sizeof(buf), "PLAYERS: %i", player_count);
    int players_x = btn_x;
    int players_y = btn_y - 28;
    
    SDL_Color black{0, 0, 0, 255};
    text->render(renderer, buf, players_x - 1, players_y - 1, black);
    text->render(renderer, buf, players_x, players_y - 1, black);
    text->render(renderer, buf, players_x + 1, players_y - 1, black);
    text->render(renderer, buf, players_x - 1, players_y, black);
    text->render(renderer, buf, players_x + 1, players_y, black);
    text->render(renderer, buf, players_x - 1, players_y + 1, black);
    text->render(renderer, buf, players_x, players_y + 1, black);
    text->render(renderer, buf, players_x + 1, players_y + 1, black);
    
    text->render(renderer, buf, players_x, players_y, label_color);
}

void UIRenderer::draw_results(const std::vector<CarRacingInfo>& cars_finished) {
    if (!text || cars_finished.empty()) return;

    const int panel_w = 800;
    const int panel_h = 600;
    const int panel_x = (screen_width - panel_w) / 2;
    const int panel_y = (screen_height - panel_h) / 2;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 230);
    SDL_Rect panel_rect = {panel_x, panel_y, panel_w, panel_h};
    SDL_RenderFillRect(renderer, &panel_rect);

    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    SDL_RenderDrawRect(renderer, &panel_rect);

    SDL_Color title_color = {255, 215, 0, 255};
    std::string title = "RACE RESULTS";
    int title_x = panel_x + (panel_w - static_cast<int>(title.size()) * 14) / 2;
    int title_y = panel_y + 30;
    text->render(renderer, title, title_x, title_y, title_color);

    SDL_Color header_color = {200, 200, 200, 255};
    text->render(renderer, "Pos", panel_x + 30, panel_y + 80, header_color);
    text->render(renderer, "Name", panel_x + 120, panel_y + 80, header_color);
    text->render(renderer, "Time", panel_x + panel_w - 180, panel_y + 80, header_color);

    int row_y = panel_y + 120;
    const int row_height = 40;
    const size_t max_rows = 15;

    for (size_t i = 0; i < cars_finished.size() && i < max_rows; ++i) {
        const auto& car_info = cars_finished[i];

        SDL_Color row_color = {255, 255, 255, 255};
        if (car_info.position == 1) {
            row_color = {255, 215, 0, 255};
        } else if (car_info.position == 2) {
            row_color = {192, 192, 192, 255};
        } else if (car_info.position == 3) {
            row_color = {205, 127, 50, 255};
        }

        char position_str[16];
        std::snprintf(position_str, sizeof(position_str), "%d.", car_info.position);
        text->render(renderer, position_str, panel_x + 30, row_y, row_color);

        std::string display_name = car_info.name;
        if (display_name.length() > 30) {
            display_name = display_name.substr(0, 27) + "...";
        }
        text->render(renderer, display_name, panel_x + 120, row_y, row_color);

        int total_seconds = static_cast<int>(car_info.time);
        int minutes = total_seconds / 60;
        int seconds = total_seconds % 60;
        int millis = static_cast<int>((car_info.time - total_seconds) * 1000);

        char time_str[32];
        std::snprintf(time_str, sizeof(time_str), "%02d:%02d.%03d", minutes, seconds, millis);
        text->render(renderer, time_str, panel_x + panel_w - 180, row_y, row_color);

        row_y += row_height;
    }
}

void UIRenderer::draw_checkpoints_info(int current_checkpoint, int total_checkpoints) {
    if (!text) return;
    
    const int panel_x = 10;
    const int panel_y = 10;
    const int panel_w = 350;
    const int panel_h = 45;
    
    int checkpoints_remaining = total_checkpoints - current_checkpoint;
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 200);
    SDL_Rect panel_rect = {panel_x, panel_y, panel_w, panel_h};
    SDL_RenderFillRect(renderer, &panel_rect);
    
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    SDL_RenderDrawRect(renderer, &panel_rect);
    
    SDL_Color title_color = {255, 215, 0, 255};
    std::string checkpoints_text = "Remaining checkpoints: " + std::to_string(checkpoints_remaining);
    text->render(renderer, checkpoints_text, panel_x + 10, panel_y + 12, title_color);
}

void UIRenderer::draw_upgrades_info(const std::map<Upgrades, int>& upgrades,
                                   SDL_Texture* icons_texture,
                                   const std::vector<UpgradeData>& upgrade_data) {
    if (!text || !icons_texture || upgrades.empty()) return;
    
    const int panel_x = 10;
    const int panel_y = 65;
    const int panel_w = 280;
    const int icon_size = 35;
    const int icon_spacing = 5;
    
    int total_icons = 0;
    for (const auto& [upgrade_type, level] : upgrades) {
        if (level > 0) total_icons += level;
    }
    
    if (total_icons == 0) return;
    
    int icons_per_row = (panel_w - 20) / (icon_size + icon_spacing);
    int icon_rows = (total_icons + icons_per_row - 1) / icons_per_row;
    int panel_h = 35 + icon_rows * (icon_size + icon_spacing);
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 200);
    SDL_Rect panel_rect = {panel_x, panel_y, panel_w, panel_h};
    SDL_RenderFillRect(renderer, &panel_rect);
    
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    SDL_RenderDrawRect(renderer, &panel_rect);
    
    SDL_Color upgrades_color = {200, 200, 200, 255};
    text->render(renderer, "UPGRADES:", panel_x + 10, panel_y + 5, upgrades_color);
    
    int current_x = panel_x + 10;
    int current_y = panel_y + 35;
    int count = 0;
    
    for (const auto& [upgrade_type, level] : upgrades) {
        if (level <= 0) continue;
        
        SDL_Rect icon_src = {0, 0, 120, 100};
        for (const auto& upgrade : upgrade_data) {
            Upgrades type = NONE_UPGRADE;
            if (upgrade.name == "mass") type = MASS_UPGRADE;
            else if (upgrade.name == "acceleration") type = ACCELERATION_UPGRADE;
            else if (upgrade.name == "handling") type = HANDLING_UPGRADE;
            else if (upgrade.name == "nitro") type = NITRO_UPGRADE;
            else if (upgrade.name == "life") type = LIFE_UPGRADE;
            else if (upgrade.name == "brakes") type = BRAKE_UPGRADE;
            
            if (type == upgrade_type) {
                icon_src = upgrade.sprite;
                break;
            }
        }
        
        for (int i = 0; i < level; ++i) {
            if (count >= icons_per_row) {
                current_x = panel_x + 10;
                current_y += icon_size + icon_spacing;
                count = 0;
            }
            
            SDL_Rect dst_rect = {current_x, current_y, icon_size, icon_size};
            SDL_RenderCopy(renderer, icons_texture, &icon_src, &dst_rect);
            
            current_x += icon_size + icon_spacing;
            count++;
        }
    }
}