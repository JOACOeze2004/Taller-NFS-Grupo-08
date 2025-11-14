#include "upgrade_phase.h"
#include <iostream>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

UpgradePhase::UpgradePhase(SDL_Renderer* renderer, SDL_Window* window, 
                           int screen_width, int screen_height, ClientHandler* _handler)
    : renderer(renderer), window(window), 
      screen_width(screen_width), screen_height(screen_height), text(nullptr),
      handler(_handler), icons_texture(nullptr), arrows_texture(nullptr),
      selected_upgrade(NONE_UPGRADE), upgrade_selected(false) {
    
    const char* default_font_path = "../assets/fonts/DejaVuSans.ttf";
    text = new TextRenderer();
    if (!text->load(default_font_path, 24) || !text->ok()) {
        std::cerr << "[UPGRADE_PHASE] Fuente no disponible" << std::endl;
        delete text;
        text = nullptr;
    }
    
    load_textures();
    init_upgrade_buttons();
    if (handler) {
        handler->clear_buttons();
        for (const auto& arrow : arrow_buttons) {
            handler->register_button(arrow.rect, arrow.type);
        }
    }

}

void UpgradePhase::load_textures() {
    SDL_Surface* icons_surface = IMG_Load("../assets/Workshop_icons.webp");
    
    if (!icons_surface) {
        std::cerr << "[UPGRADE_PHASE] Error loading Workshop_icons.webp: " << IMG_GetError() << std::endl;
    } else {
        icons_texture = SDL_CreateTextureFromSurface(renderer, icons_surface);
        SDL_FreeSurface(icons_surface);
        if (!icons_texture) {
            std::cerr << "[UPGRADE_PHASE] Error creating icons texture: " << SDL_GetError() << std::endl;
        }
    }
    
    SDL_Surface* arrows_surface = IMG_Load("../assets/workshop_arrow_spritesheet.png");
    if (!arrows_surface) {
        std::cerr << "[UPGRADE_PHASE] Error loading workshop_arrow_spritesheet.png: " << IMG_GetError() << std::endl;
    } else {
        arrows_texture = SDL_CreateTextureFromSurface(renderer, arrows_surface);
        SDL_FreeSurface(arrows_surface);
        if (!arrows_texture) {
            std::cerr << "[UPGRADE_PHASE] Error creating arrows texture: " << SDL_GetError() << std::endl;
        }
    }
}

void UpgradePhase::init_upgrade_buttons() {
    upgrade_buttons.clear();
    arrow_buttons.clear();
    
    const int button_width = 280;
    const int button_height = 80;
    const int spacing = 20;
    const int start_y = 150;
    const int icon_size = 48;
    const int arrow_size = 24;
    
    int start_x = (screen_width - button_width) / 2;
    
    auto addUpgrade = [&](int order, Upgrades up_type, const char* title, const char* desc, SDL_Rect icon_src) {
        UpgradeButton btn;
        btn.rect = {start_x, start_y + order * (button_height + spacing), button_width, button_height};
        btn.upgrade_type = up_type;
        btn.title = title;
        btn.description = desc;
        btn.icon_src_rect = icon_src;
        upgrade_buttons.push_back(btn);

        int center_x = btn.rect.x + btn.rect.w / 2;
        int center_y = btn.rect.y + 15;
        SDL_Rect left_arrow = { center_x - icon_size / 2 - arrow_size - 5, center_y, arrow_size, arrow_size };
        SDL_Rect right_arrow = { center_x + icon_size / 2 + 5, center_y, arrow_size, arrow_size };

        switch (up_type) {
            case LIFE:
                arrow_buttons.push_back({left_arrow, BUTTON_LIFE_DOWN});
                arrow_buttons.push_back({right_arrow, BUTTON_LIFE_UP});
                break;
            case NITRO:
                arrow_buttons.push_back({left_arrow, BUTTON_NITRO_DOWN});
                arrow_buttons.push_back({right_arrow, BUTTON_NITRO_UP});
                break;
            case ACCELERATION_UPGRADE:
                arrow_buttons.push_back({left_arrow, BUTTON_ACCELERATION_DOWN});
                arrow_buttons.push_back({right_arrow, BUTTON_ACCELERATION_UP});
                break;
            case HANDLING_UPGRADE:
                arrow_buttons.push_back({left_arrow, BUTTON_HANDLING_DOWN});
                arrow_buttons.push_back({right_arrow, BUTTON_HANDLING_UP});
                break;
            case MASS_UPGRADE: 
                arrow_buttons.push_back({left_arrow, BUTTON_CONTROL_DOWN});
                arrow_buttons.push_back({right_arrow, BUTTON_CONTROL_UP});
                break;
            case BRAKE_UPGRADE: 
                arrow_buttons.push_back({left_arrow, BUTTON_VELOCITY_DOWN});
                arrow_buttons.push_back({right_arrow, BUTTON_VELOCITY_UP});
                break;
            default:
                break;
        }
    };
    
    addUpgrade(3, NITRO, "NITRO", "Impulso de velocidad temporal", SDL_Rect{192,0,64,64});
    addUpgrade(4, LIFE, "VIDA", "Aumenta la resistencia del auto", SDL_Rect{256,0,64,64});
    addUpgrade(1, ACCELERATION_UPGRADE, "ACELERACION", "Acelera mas rapido", SDL_Rect{64,0,64,64});
    addUpgrade(0, MASS_UPGRADE, "MASA", "Aumenta la masa del auto", SDL_Rect{0,0,64,64});
    addUpgrade(5, BRAKE_UPGRADE, "FRENOS", "Mejora la capacidad de frenado", SDL_Rect{320,0,64,64});
    addUpgrade(2, HANDLING_UPGRADE, "MANEJO", "Mejora el control del auto", SDL_Rect{128,0,64,64});
}

void UpgradePhase::render_background() {
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 100);
    SDL_Rect fullscreen = {0, 0, screen_width, screen_height};
    SDL_RenderFillRect(renderer, &fullscreen);
}

void UpgradePhase::render_title() {
    if (!text) return;
    
    std::string title = "MEJORAS DISPONIBLES";
    SDL_Color title_color = {255, 215, 0, 255}; 
    
    int title_x = (screen_width / 2) - 200;
    int title_y = 100;
    
    text->render(renderer, title, title_x, title_y, title_color);
}

void UpgradePhase::render_upgrade_buttons() {
    SDL_Rect left_arrow_src = {0, 0, 32, 32};   // Placeholder - ajustar coordenadas
    SDL_Rect right_arrow_src = {32, 0, 32, 32}; // Placeholder - ajustar coordenadas
    
    for (auto& button : upgrade_buttons) {
        SDL_Color bg_color = {80, 120, 200, 255};
        SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        SDL_RenderFillRect(renderer, &button.rect);
        SDL_SetRenderDrawColor(renderer, 160, 190, 230, 255);
        SDL_RenderDrawRect(renderer, &button.rect);
        
        const int icon_size = 48;
        const int arrow_size = 24;
        const int center_x = button.rect.x + button.rect.w / 2;
        const int center_y = button.rect.y + 15;
        
        // derive arrow rects (same math as init) for hover highlighting
        SDL_Rect left_arrow = { center_x - icon_size / 2 - arrow_size - 5, center_y, arrow_size, arrow_size };
        SDL_Rect right_arrow = { center_x + icon_size / 2 + 5, center_y, arrow_size, arrow_size };
        bool hover_left = handler && handler->is_mouse_over_button(left_arrow);
        bool hover_right = handler && handler->is_mouse_over_button(right_arrow);
        
        if (arrows_texture) {
            if (hover_left) SDL_SetTextureColorMod(arrows_texture, 200, 80, 80);
            SDL_RenderCopy(renderer, arrows_texture, &left_arrow_src, &left_arrow);
            if (hover_left) SDL_SetTextureColorMod(arrows_texture, 255, 255, 255);
        }
        
        if (icons_texture) {
            SDL_Rect icon_dst = { center_x - icon_size / 2, center_y, icon_size, icon_size };
            SDL_RenderCopy(renderer, icons_texture, &button.icon_src_rect, &icon_dst);
        }
        
        if (arrows_texture) {
            if (hover_right) SDL_SetTextureColorMod(arrows_texture, 80, 200, 80);
            SDL_RenderCopy(renderer, arrows_texture, &right_arrow_src, &right_arrow);
            if (hover_right) SDL_SetTextureColorMod(arrows_texture, 255, 255, 255);
        }
        
        if (text) {
            SDL_Color text_color = {255,255,255,255};
            int title_x = button.rect.x + (button.rect.w - button.title.length() * 6) / 2;
            int title_y = button.rect.y + 10;
            text->render(renderer, button.title, title_x, title_y, text_color);
        }
    }
}

void UpgradePhase::render_instructions() {
    if (!text) return;
    
    std::string instruction = "Haz click en una mejora para seleccionarla";
    SDL_Color color = {180, 180, 180, 255};
    
    int x = (screen_width / 2) - 250;
    int y = screen_height - 100;
    
    text->render(renderer, instruction, x, y, color);
}

void UpgradePhase::render() {
    //SDL_RenderClear(renderer);
    
    render_background();
    render_title();
    render_upgrade_buttons();
    render_instructions();
    
}


UpgradePhase::~UpgradePhase() {
    if (handler) {
        handler->clear_buttons();
    }

    if (icons_texture) {
        SDL_DestroyTexture(icons_texture);
        icons_texture = nullptr;
    }
    
    if (arrows_texture) {
        SDL_DestroyTexture(arrows_texture);
        arrows_texture = nullptr;
    }

    if (text) {
        delete text;
        text = nullptr;
    }
}