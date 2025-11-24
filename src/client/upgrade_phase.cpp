#include "upgrade_phase.h"
#include <iostream>
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
    
    int start_x = (screen_width - UPGRADE_BUTTON_WIDTH) / 2;

    for (const auto& upgrade : upgrade_data) {
        UpgradeButton btn;
        btn.rect = {
            start_x,
            UPGRADE_START_Y + upgrade.order * (UPGRADE_BUTTON_HEIGHT + UPGRADE_BUTTON_SPACING),
            UPGRADE_BUTTON_WIDTH,
            UPGRADE_BUTTON_HEIGHT
        };
        btn.upgrade_type = NONE_UPGRADE;
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
    int title_y = 30;
    int est_width = static_cast<int>(title.size()) * 10;
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
    
    int est_width = static_cast<int>(info.size()) * 9;
    int info_x = (screen_width - est_width) / 2;
    int info_y = 50;
    text->render(renderer, info, info_x, info_y, COLOR_UPGRADE_INFO);
}

void UpgradePhase::render_upgrade_buttons() {
    
    if (handler) {
        handler->clear_buttons();
        for (const auto& arrow : arrow_buttons) {
            handler->register_button(arrow.rect, arrow.type);
        }
    }

    SDL_Rect arrow_src = arrow_data.sprite;
    for (size_t i = 0; i < upgrade_buttons.size(); ++i) {
        auto& button = upgrade_buttons[i];
        SDL_SetRenderDrawColor(renderer, COLOR_UPGRADE_BUTTON_BG.r, COLOR_UPGRADE_BUTTON_BG.g, 
                              COLOR_UPGRADE_BUTTON_BG.b, COLOR_UPGRADE_BUTTON_BG.a);
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
                button.rect.y + (button.rect.h - dest_h)/2,
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
            int est_title_width = static_cast<int>(button.title.size()) * 8;
            int title_x = button.rect.x + (button.rect.w - est_title_width) / 2;
            text->render(renderer, button.title, title_x, title_y, COLOR_WHITE);
            
            int desc_y = button.rect.y + button.rect.h - UPGRADE_DESC_Y_OFFSET;
            int est_desc_width = static_cast<int>(button.description.size()) * 7;
            int desc_x = button.rect.x + (button.rect.w - est_desc_width) / 2;
            text->render(renderer, button.description, desc_x, desc_y, COLOR_UPGRADE_DESC);
        }
    }
}

void UpgradePhase::render_instructions() {
    if (!text) return;
    std::string instruction = "Use LEFT/RIGHT arrows to downgrade/upgrade";
    int est_width = static_cast<int>(instruction.size()) * 10;
    int x = (screen_width - est_width) / 2;
    int y = screen_height - 30;
    text->render(renderer, instruction, x, y, COLOR_UPGRADE_INFO);
}

void UpgradePhase::render(int remaining_upgrades) {
    
    render_background();
    render_title();
    render_remaining_upgrades(remaining_upgrades);
    render_upgrade_buttons();
    render_instructions();
    
}

UpgradePhase::~UpgradePhase() {
    if (handler) {
        handler->clear_buttons();
    }
}