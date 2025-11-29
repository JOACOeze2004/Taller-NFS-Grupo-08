#include "client_handler.h"
#include "ClientQuitException.h"
#include "../common/constants.h"

ClientHandler::ClientHandler(InputParser& _parser): parser(_parser), mouse_was_down(false) {
    initialize_key_map();
    initialize_button_map();
    initialize_cheat_map();
}

void ClientHandler::handle_event() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            try {
                parser.parse_command(SEND_DISCONNECT);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            } catch (...) {}
            throw ClientQuitException();
        }
        process_event(event);
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q){
            try {
                parser.parse_command(SEND_DISCONNECT);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            } catch (...) {}
            throw ClientQuitException();
        }
    }
    update();
}

void ClientHandler::register_button(const SDL_Rect& rect, ButtonType type) {
    buttons.emplace_back(rect, type);
}

void ClientHandler::clear_buttons() {
    buttons.clear();
}

void ClientHandler::handle_button_action(ButtonType type) {
    auto it = button_command_map.find(type);
    if (it != button_command_map.end()) {
        parser.parse_command(it->second);
    }
}

void ClientHandler::initialize_button_map() {
    button_command_map[BUTTON_READY] = SEND_READY_TO_PLAY;
    button_command_map[BUTTON_LIFE_UP] = SEND_LIFE_UPGRADE;
    button_command_map[BUTTON_LIFE_DOWN] = SEND_LIFE_DOWNGRADE;
    button_command_map[BUTTON_VELOCITY_UP] = SEND_BRAKE_UPGRADE;
    button_command_map[BUTTON_VELOCITY_DOWN] = SEND_BRAKE_DOWNGRADE;
    button_command_map[BUTTON_ACCELERATION_UP] = SEND_ACCELERATION_UPGRADE;
    button_command_map[BUTTON_ACCELERATION_DOWN] = SEND_ACCELERATION_DOWNGRADE;
    button_command_map[BUTTON_HANDLING_UP] = SEND_HANDLING_UPGRADE;
    button_command_map[BUTTON_HANDLING_DOWN] = SEND_HANDLING_DOWNGRADE;
    button_command_map[BUTTON_CONTROL_UP] = SEND_MASS_UPGRADE;
    button_command_map[BUTTON_CONTROL_DOWN] = SEND_MASS_DOWNGRADE;
    button_command_map[BUTTON_NITRO_UP] = SEND_NITRO_UPGRADE;
    button_command_map[BUTTON_NITRO_DOWN] = SEND_NITRO_DOWNGRADE;
}

void ClientHandler::initialize_cheat_map() {
    cheat_command_map[SDLK_z] = SEND_WIN_RACE_CHEAT;
    cheat_command_map[SDLK_x] = SEND_RESTORE_LIFE_CHEAT;
    cheat_command_map[SDLK_c] = SEND_INFINITE_LIFE_CHEAT;
    cheat_command_map[SDLK_v] = SEND_LOSE_RACE_CHEAT;
    cheat_command_map[SDLK_b] = SEND_INFINITE_NITRO_CHEAT;
    cheat_command_map[SDLK_a] = SEND_NEXT_CHECKPOINT;
    cheat_command_map[SDLK_f] = SEND_PASS_TO_FINAL_RACE;
    cheat_command_map[SDLK_q] = SEND_DISCONNECT;

}

bool ClientHandler::is_point_in_rect(int x, int y, const SDL_Rect& rect) const {
    return (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h);
}

void ClientHandler::process_mouse_click(int x, int y) {
    for (auto& button : buttons) {
        if (button.enabled && is_point_in_rect(x, y, button.rect)) {
            handle_button_action(button.type);  
            return;
        }
    }
}

void ClientHandler::process_mouse_event(const SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        if (!mouse_was_down) {
            process_mouse_click(event.button.x, event.button.y);
            mouse_was_down = true;
        }
        return;
    }
    
    if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
        mouse_was_down = false;
    }
}

void ClientHandler::process_keyboard_event(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        SDL_Keymod mod = SDL_GetModState();
        if (mod & KMOD_CTRL) {
            auto it = cheat_command_map.find(event.key.keysym.sym);
            if (it != cheat_command_map.end()) {
                parser.parse_command(it->second);
            }
            return;
        }
        key_state[event.key.keysym.sym] = true;
        return;
    }
    
    if (event.type == SDL_KEYUP) {
        key_state[event.key.keysym.sym] = false;
    }
}

void ClientHandler::process_event(const SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
        process_mouse_event(event);
        return;
    }
    
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        process_keyboard_event(event);
    }
}

void ClientHandler::initialize_key_map() {
    key_map[SDLK_w] = [this]() { parser.parse_command(SEND_ACCELERATE); };
    key_map[SDLK_a] = [this]() { parser.parse_command(SEND_ROTATE_LEFT); };
    key_map[SDLK_s] = [this]() { parser.parse_command(SEND_BRAKE); };
    key_map[SDLK_d] = [this]() { parser.parse_command(SEND_ROTATE_RIGHT); };
    key_map[SDLK_n] = [this]() { parser.parse_command(SEND_USE_NITRO); };
    key_map[SDLK_r] = [this]() { parser.parse_command(SEND_READY_TO_PLAY); };
}

void ClientHandler::update() {
    for (auto& [key, func] : key_map) {
        if (key_state[key]) {func();}
    }
}

bool ClientHandler::is_mouse_over_button(const SDL_Rect& rect) const {
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    return is_point_in_rect(mouse_x, mouse_y, rect);
}

ClientHandler::~ClientHandler() {
    clear_buttons();
}