#include "upgrade_phase.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

UpgradePhase::UpgradePhase(SDL_Renderer* renderer, SDL_Window* window, 
                           int screen_width, int screen_height, ClientHandler* _handler,
                           ResourceManager* res)
    : renderer(renderer), window(window), 
      screen_width(screen_width), screen_height(screen_height), 
      handler(_handler), resources(res),
      icons_texture(nullptr), arrows_texture(nullptr),
      selected_upgrade(NONE_UPGRADE), upgrade_selected(false) {
    
    text = std::make_unique<TextRenderer>(DEFAULT_FONT_PATH, UPGRADE_FONT_SIZE);
    if (!text->ok()) {
        std::cerr << "[UPGRADE_PHASE] Fuente no disponible" << std::endl;
        text.reset();
    }
    
    load_textures();
    load_upgrade_sprites();
    init_upgrade_buttons();
    if (handler) {
        handler->clear_buttons();
        for (const auto& arrow : arrow_buttons) {
            handler->register_button(arrow.rect, arrow.type);
        }
    }

}

void UpgradePhase::load_textures() {
    icons_texture = resources->load(WORKSHOP_ICONS_PATH);
    arrows_texture = resources->load(WORKSHOP_ARROWS_PATH);
}

void UpgradePhase::load_upgrade_sprites() {
    const std::string yaml_path = "../src/client/workshop_sprites.yaml";
    upgrade_data = SpriteLoader::loadUpgradeSprites(yaml_path);
    arrow_data = SpriteLoader::loadArrowSprite(yaml_path);
}

Upgrades UpgradePhase::getUpgradeType(const std::string& upgrade_name) {
    if (upgrade_name == "mass") return MASS_UPGRADE;
    if (upgrade_name == "acceleration") return ACCELERATION_UPGRADE;
    if (upgrade_name == "handling") return HANDLING_UPGRADE;
    if (upgrade_name == "nitro") return NITRO_UPGRADE;
    if (upgrade_name == "life") return LIFE_UPGRADE;
    if (upgrade_name == "brakes") return BRAKE_UPGRADE;
    return NONE_UPGRADE;
}

ButtonType UpgradePhase::getButtonType(const std::string& button_name) {
    if (button_name == "BUTTON_LIFE_UP") return BUTTON_LIFE_UP;
    if (button_name == "BUTTON_LIFE_DOWN") return BUTTON_LIFE_DOWN;
    if (button_name == "BUTTON_VELOCITY_UP") return BUTTON_VELOCITY_UP;
    if (button_name == "BUTTON_VELOCITY_DOWN") return BUTTON_VELOCITY_DOWN;
    if (button_name == "BUTTON_ACCELERATION_UP") return BUTTON_ACCELERATION_UP;
    if (button_name == "BUTTON_ACCELERATION_DOWN") return BUTTON_ACCELERATION_DOWN;
    if (button_name == "BUTTON_HANDLING_UP") return BUTTON_HANDLING_UP;
    if (button_name == "BUTTON_HANDLING_DOWN") return BUTTON_HANDLING_DOWN;
    if (button_name == "BUTTON_CONTROL_UP") return BUTTON_CONTROL_UP;
    if (button_name == "BUTTON_CONTROL_DOWN") return BUTTON_CONTROL_DOWN;
    if (button_name == "BUTTON_NITRO_UP") return BUTTON_NITRO_UP;
    if (button_name == "BUTTON_NITRO_DOWN") return BUTTON_NITRO_DOWN;
    return BUTTON_READY;
}

void UpgradePhase::init_upgrade_buttons() {
    upgrade_buttons.clear();
    arrow_buttons.clear();
    
    int total_width = 2 * UPGRADE_BUTTON_WIDTH + UPGRADE_COLUMN_SPACING;
    int left_column_x = (screen_width - total_width) / 2;
    int right_column_x = left_column_x + UPGRADE_BUTTON_WIDTH + UPGRADE_COLUMN_SPACING;

    for (size_t i = 0; i < upgrade_data.size(); ++i) {
        const auto& upgrade = upgrade_data[i];
        UpgradeButton btn;
        
        bool is_left_column = (i < 3);
        int column_x = is_left_column ? left_column_x : right_column_x;
        int row_in_column = is_left_column ? i : (i - 3);
        
        btn.rect = {
            column_x,
            UPGRADE_START_Y + row_in_column * (UPGRADE_BUTTON_HEIGHT + UPGRADE_BUTTON_SPACING),
            UPGRADE_BUTTON_WIDTH,
            UPGRADE_BUTTON_HEIGHT
        };

        btn.upgrade_type = getUpgradeType(upgrade.name);
        btn.title = upgrade.title;
        btn.description = upgrade.description;
        btn.icon_src_rect = upgrade.sprite;
        upgrade_buttons.push_back(btn);

        int center_x = btn.rect.x + btn.rect.w / 2;
        int arrow_gap = UPGRADE_ICON_SIZE + UPGRADE_ARROW_GAP;
        int arrow_y = btn.rect.y + (btn.rect.h - UPGRADE_ARROW_SIZE) / 2;

        SDL_Rect left_arrow  = {
            center_x - arrow_gap - UPGRADE_ARROW_SIZE,
            arrow_y,
            UPGRADE_ARROW_SIZE,
            UPGRADE_ARROW_SIZE
        };
        SDL_Rect right_arrow = {
            center_x + arrow_gap,
            arrow_y,
            UPGRADE_ARROW_SIZE,
            UPGRADE_ARROW_SIZE
        };

        arrow_buttons.push_back({left_arrow, getButtonType(upgrade.button_left)});
        arrow_buttons.push_back({right_arrow, getButtonType(upgrade.button_right)});
    }
}

void UpgradePhase::render_background() {
    SDL_SetRenderDrawColor(renderer, COLOR_UPGRADE_BG.r, COLOR_UPGRADE_BG.g, 
                          COLOR_UPGRADE_BG.b, COLOR_UPGRADE_BG.a);
    SDL_Rect fullscreen = {0, 0, screen_width, screen_height};
    SDL_RenderFillRect(renderer, &fullscreen);
}

void UpgradePhase::render_title() {
    if (!text) return;
    std::string title = "AVAILABLE UPGRADES";
    int title_y = 20;
    int est_width = static_cast<int>(title.size()) * 14;
    int title_x = (screen_width - est_width) / 2;
    text->render(renderer, title, title_x, title_y, COLOR_UPGRADE_TITLE);
}

void UpgradePhase::render_remaining_upgrades(int remaining_upgrades) {
    if (!text) return;
    
    std::string info;
    if (remaining_upgrades == 1) {
        info = "You have 1 upgrade point to spend";
    } else if (remaining_upgrades == 0) {
        info = "No upgrade points remaining";
    } else {
        info = "You have " + std::to_string(remaining_upgrades) + " upgrade points to spend";
    }
    
    int est_width = static_cast<int>(info.size()) * 11;
    int info_x = (screen_width - est_width) / 2;
    int info_y = 55;
    text->render(renderer, info, info_x, info_y, COLOR_UPGRADE_INFO);
}

void UpgradePhase::render_upgrade_buttons(const std::map<Upgrades, std::chrono::seconds>& prices, const std::map<Upgrades, int>& current_upgrades) {
    
    if (handler) {
        handler->clear_buttons();
        for (const auto& arrow : arrow_buttons) {
            handler->register_button(arrow.rect, arrow.type);
        }
    }

    SDL_Rect arrow_src = arrow_data.sprite;
    for (size_t i = 0; i < upgrade_buttons.size(); ++i) {
        auto& button = upgrade_buttons[i];
        
        int upgrade_level = 0;
        auto it = current_upgrades.find(button.upgrade_type);
        if (it != current_upgrades.end()) {
            upgrade_level = it->second;
        }
        
        SDL_Color bg_color = COLOR_UPGRADE_BUTTON_BG;
        if (upgrade_level > 0) {
            if (upgrade_level == 1) {
                bg_color = {120, 160, 220, 220};
            } else if (upgrade_level == 2) {
                bg_color = {180, 180, 200, 220};
            } else {
                bg_color = {220, 200, 120, 220};
            }
        }
        
        SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        SDL_RenderFillRect(renderer, &button.rect);
        SDL_SetRenderDrawColor(renderer, COLOR_UPGRADE_BUTTON_BORDER.r, COLOR_UPGRADE_BUTTON_BORDER.g, 
                              COLOR_UPGRADE_BUTTON_BORDER.b, COLOR_UPGRADE_BUTTON_BORDER.a);
        SDL_RenderDrawRect(renderer, &button.rect);

        
        if (icons_texture) {
            int tex_w = 0, tex_h = 0;
            SDL_QueryTexture(icons_texture, nullptr, nullptr, &tex_w, &tex_h);

            SDL_Rect src = button.icon_src_rect;
            
            if (src.x < 0) src.x = 0;
            if (src.y < 0) src.y = 0;
            if (src.x + src.w > tex_w) src.w = tex_w - src.x;
            if (src.y + src.h > tex_h) src.h = tex_h - src.y;

            int max_h = button.rect.h - UPGRADE_ICON_PADDING * 2;
            if (max_h < 8) max_h = button.rect.h;
            
            int dest_h = (UPGRADE_ICON_SIZE < max_h ? UPGRADE_ICON_SIZE : max_h);
            
            double aspect = (src.h == 0) ? 1.0 : (double)src.w / (double)src.h;
            int dest_w = static_cast<int>(aspect * dest_h);
            
            if (dest_w > button.rect.w - UPGRADE_ICON_PADDING * 2) {
                dest_w = button.rect.w - UPGRADE_ICON_PADDING * 2;
                dest_h = static_cast<int>( (double)dest_w / (aspect == 0 ? 1.0 : aspect) );
            }
            SDL_Rect icon_dst = {
                button.rect.x + (button.rect.w - dest_w)/2,
                button.rect.y + (button.rect.h - dest_h)/2 - 10,
                dest_w,
                dest_h
            };
            SDL_RenderCopy(renderer, icons_texture, &src, &icon_dst);
        }

        
        SDL_Rect left_arrow = arrow_buttons[2*i].rect;
        SDL_Rect right_arrow = arrow_buttons[2*i + 1].rect;
        bool hover_left = handler && handler->is_mouse_over_button(left_arrow);
        bool hover_right = handler && handler->is_mouse_over_button(right_arrow);

        if (arrows_texture) {
            if (hover_left) SDL_SetTextureColorMod(arrows_texture, 220, 100, 100);
            SDL_RenderCopyEx(renderer, arrows_texture, &arrow_src, &left_arrow, 0.0, nullptr, SDL_FLIP_HORIZONTAL);
            if (hover_left) SDL_SetTextureColorMod(arrows_texture, 255,255,255);
            if (hover_right) SDL_SetTextureColorMod(arrows_texture, 100, 220, 100);
            SDL_RenderCopyEx(renderer, arrows_texture, &arrow_src, &right_arrow, 0.0, nullptr, SDL_FLIP_NONE);
            if (hover_right) SDL_SetTextureColorMod(arrows_texture, 255,255,255);
        }

        if (text) {
            int title_y = button.rect.y + UPGRADE_TITLE_Y_OFFSET;
            int est_title_width = static_cast<int>(button.title.size()) * 14;
            int title_x = button.rect.x + (button.rect.w - est_title_width) / 2;
            
            text->render_with_outline(renderer, button.title, title_x, title_y, COLOR_WHITE, COLOR_BLACK);
            
            int desc_y = button.rect.y + button.rect.h - UPGRADE_DESC_Y_OFFSET;
            int est_desc_width = static_cast<int>(button.description.size()) * 11;
            int desc_x = button.rect.x + (button.rect.w - est_desc_width) / 2;
            
            text->render_with_outline(renderer, button.description, desc_x, desc_y, COLOR_WHITE, COLOR_BLACK);
            
            

            auto it = prices.find(button.upgrade_type);
            if (it != prices.end()) {
                int seconds = static_cast<int>(it->second.count());
                std::string price_text = "Cost: " + std::to_string(seconds) + "s";
                int price_y = desc_y + 28;
                int max_y = button.rect.y + button.rect.h - 10;
                if (price_y > max_y) price_y = max_y;
                
                int est_price_width = static_cast<int>(price_text.size()) * 11;
                int price_x = button.rect.x + (button.rect.w - est_price_width) / 2;
                
                text->render_with_outline(renderer, price_text, price_x, price_y, COLOR_GOLD, COLOR_BLACK);
            }
        }
    }
}

void UpgradePhase::render_instructions() {
    if (!text) return;
    std::string instruction = "Use LEFT/RIGHT arrows to downgrade/upgrade";
    int est_width = static_cast<int>(instruction.size()) * 12;
    int x = (screen_width - est_width) / 2;
    int y = screen_height - 40;
    text->render(renderer, instruction, x, y, COLOR_UPGRADE_INFO);
}

void UpgradePhase::render_actual_upgrades(std::map<Upgrades, int> upgrades){
    if (!icons_texture || !text) return;
    
    const int icon_size = 75;
    const int icon_spacing = 15;
    const int y_position = screen_height - 130;
    
    SDL_Rect clear_area = {0, y_position - 40, screen_width, icon_size + 50};
    SDL_SetRenderDrawColor(renderer, COLOR_UPGRADE_BG.r, COLOR_UPGRADE_BG.g, 
                          COLOR_UPGRADE_BG.b, COLOR_UPGRADE_BG.a);
    SDL_RenderFillRect(renderer, &clear_area);
    
    SDL_Color title_color = {255, 215, 0, 255};
    std::string title = "CURRENT UPGRADES:";
    int title_y = y_position - 35;
    
    int total_width = 2 * UPGRADE_BUTTON_WIDTH + UPGRADE_COLUMN_SPACING;
    int left_column_x = (screen_width - total_width) / 2;
    int right_column_x = left_column_x + UPGRADE_BUTTON_WIDTH + UPGRADE_COLUMN_SPACING;
    
    int title_width = static_cast<int>(title.size()) * 12;
    int title_x = right_column_x + (UPGRADE_BUTTON_WIDTH - title_width) / 2;
    text->render(renderer, title, title_x, title_y, title_color);
    
    if (upgrades.empty()) return;
    
    int total_icons = 0;
    for (const auto& [upgrade_type, level] : upgrades) {
        if (level > 0) total_icons += level;
    }
    int icons_width = total_icons * (icon_size + icon_spacing) - icon_spacing;
    int start_x = right_column_x + (UPGRADE_BUTTON_WIDTH - icons_width) / 2;
    int current_x = start_x;
    
    for (const auto& [upgrade_type, level] : upgrades) {
        if (level <= 0) continue;
        
        SDL_Rect icon_src = {0, 0, 100, 100};
        for (const auto& button : upgrade_buttons) {
            if (button.upgrade_type == upgrade_type) {
                icon_src = button.icon_src_rect;
                break;
            }
        }
        
        for (int i = 0; i < level; ++i) {
            SDL_Rect dst_rect = {current_x, y_position, icon_size, icon_size};
            SDL_RenderCopy(renderer, icons_texture, &icon_src, &dst_rect);
            
            current_x += icon_size + icon_spacing;
        }
    }
}

void UpgradePhase::render_penalty_time(int penalty_seconds) {
    if (!text) return;
    
    const int y_position = screen_height - 130;
    const int title_y = y_position - 35;
    const int value_y = y_position + 20;
    
    std::string title = "PENALTY TIME:";
    std::string penalty_text = std::to_string(penalty_seconds) + "s";
    
    int total_width_calc = 2 * UPGRADE_BUTTON_WIDTH + UPGRADE_COLUMN_SPACING;
    int left_column_x = (screen_width - total_width_calc) / 2;
    
    int title_width = static_cast<int>(title.size()) * 12;
    int penalty_width = static_cast<int>(penalty_text.size()) * 12;
    
    int title_x = left_column_x + (UPGRADE_BUTTON_WIDTH - title_width) / 2;
    int penalty_x = left_column_x + (UPGRADE_BUTTON_WIDTH - penalty_width) / 2;
    
    SDL_Color title_color = {255, 215, 0, 255};
    text->render(renderer, title, title_x, title_y, title_color);
    
    text->render_with_outline(renderer, penalty_text, penalty_x, value_y, COLOR_WHITE, COLOR_BLACK);
}

void UpgradePhase::render(const Snapshot& snapshot) {
    
    render_background();
    render_title();
    render_remaining_upgrades(snapshot.cars.at(snapshot.id).remaining_upgrades);
    render_upgrade_buttons(snapshot.prices, snapshot.upgrades);
    render_instructions();
    render_actual_upgrades(snapshot.upgrades);
    render_penalty_time(snapshot.upgrade_penalty_seconds);
}

UpgradePhase::~UpgradePhase() {
    if (handler) {
        handler->clear_buttons();
    }
}