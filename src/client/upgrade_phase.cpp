#include "upgrade_phase.h"
#include <iostream>
#include <SDL2/SDL_ttf.h>

UpgradePhase::UpgradePhase(SDL_Renderer* renderer, SDL_Window* window, 
                           int screen_width, int screen_height)
    : renderer(renderer), window(window), 
      screen_width(screen_width), screen_height(screen_height), text(nullptr),
      selected_upgrade(NONE_UPGRADE), upgrade_selected(false) {
    
    // inicializo otro pensando en capaz usar fuentes diferentes, si no reutilizar el que ya esta
    const char* default_font_path = "../assets/fonts/DejaVuSans.ttf";
    text = new TextRenderer();
    if (!text->load(default_font_path, 24) || !text->ok()) {
        std::cerr << "[UPGRADE_PHASE] Fuente no disponible" << std::endl;
        delete text;
        text = nullptr;
    }
    
    init_upgrade_buttons();
}

UpgradePhase::~UpgradePhase() {
    if (text) {
        delete text;
        text = nullptr;
    }
}

void UpgradePhase::init_upgrade_buttons() {
    upgrade_buttons.clear();
    
    const int button_width = 280;
    const int button_height = 80;
    const int spacing = 20;
    const int start_y = 150;
    
    int start_x = (screen_width - button_width) / 2;
    
    UpgradeButton speed_btn;
    speed_btn.rect = {start_x, start_y, button_width, button_height};
    speed_btn.upgrade_type = SPEED;
    speed_btn.title = "VELOCIDAD MAXIMA";
    speed_btn.description = "Aumenta la velocidad maxima";
    speed_btn.is_hovered = false;
    upgrade_buttons.push_back(speed_btn);
    
    UpgradeButton accel_btn;
    accel_btn.rect = {start_x, start_y + (button_height + spacing), button_width, button_height};
    accel_btn.upgrade_type = ACCELERATION_UPGRADE;
    accel_btn.title = "ACELERACION";
    accel_btn.description = "Acelera mas rapido";
    accel_btn.is_hovered = false;
    upgrade_buttons.push_back(accel_btn);
    
    UpgradeButton handling_btn;
    handling_btn.rect = {start_x, start_y + 2 * (button_height + spacing), button_width, button_height};
    handling_btn.upgrade_type = HANDLING_UPGRADE;
    handling_btn.title = "MANEJO";
    handling_btn.description = "Mejora el control del auto";
    handling_btn.is_hovered = false;
    upgrade_buttons.push_back(handling_btn);
    
    UpgradeButton nitro_btn;
    nitro_btn.rect = {start_x, start_y + 3 * (button_height + spacing), button_width, button_height};
    nitro_btn.upgrade_type = NITRO;
    nitro_btn.title = "NITRO";
    nitro_btn.description = "Impulso de velocidad temporal";
    nitro_btn.is_hovered = false;
    upgrade_buttons.push_back(nitro_btn);
    
    UpgradeButton life_btn;
    life_btn.rect = {start_x, start_y + 4 * (button_height + spacing), button_width, button_height};
    life_btn.upgrade_type = LIFE;
    life_btn.title = "VIDA";
    life_btn.description = "Aumenta la resistencia del auto";
    life_btn.is_hovered = false;
    upgrade_buttons.push_back(life_btn);
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
    for (auto& button : upgrade_buttons) {
        SDL_Color button_color;
        SDL_Color text_color = {255, 255, 255, 255};
        
        if (button.is_hovered) {
            button_color = {100, 150, 255, 255};
        } else {
            button_color = {50, 80, 150, 255}; 
        }
        
        SDL_SetRenderDrawColor(renderer, button_color.r, button_color.g, button_color.b, button_color.a);
        SDL_RenderFillRect(renderer, &button.rect);
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &button.rect);
        
        if (text) {
            int title_x = button.rect.x + 15;
            int title_y = button.rect.y + 15;
            text->render(renderer, button.title, title_x, title_y, text_color);
            
            SDL_Color desc_color = {200, 200, 200, 255};
            int desc_x = button.rect.x + 15;
            int desc_y = button.rect.y + 45;
            text->render(renderer, button.description, desc_x, desc_y, desc_color);
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
    
    SDL_RenderPresent(renderer);
}

bool UpgradePhase::is_point_in_rect(int x, int y, const SDL_Rect& rect) {
    return (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h);
}

void UpgradePhase::handle_mouse_motion(int mouse_x, int mouse_y) {
    for (auto& button : upgrade_buttons) {
        button.is_hovered = is_point_in_rect(mouse_x, mouse_y, button.rect);
    }
}

void UpgradePhase::handle_mouse_click(int mouse_x, int mouse_y) {
    for (const auto& button : upgrade_buttons) {
        if (is_point_in_rect(mouse_x, mouse_y, button.rect)) {
            selected_upgrade = button.upgrade_type;
            upgrade_selected = true;
            std::cout << "[UPGRADE_PHASE] Mejora seleccionada: " << button.title << std::endl;
            break;
        }
    }
}

void UpgradePhase::handle_event(const SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        handle_mouse_motion(event.motion.x, event.motion.y);
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            handle_mouse_click(event.button.x, event.button.y);
        }
    }
}

Upgrades UpgradePhase::show_and_wait_selection() {
    upgrade_selected = false;
    selected_upgrade = NONE_UPGRADE;
    
    SDL_Event event;
    
    while (!upgrade_selected) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                upgrade_selected = true;
                selected_upgrade = NONE_UPGRADE;
                break;
            }
            
            handle_event(event);
        }
        
        render();
        
        // Pequeño delay para no consumir demasiada CPU
        SDL_Delay(16); // ~60 FPS
    }
    
    return selected_upgrade;
}
