#include "client_handler.h"
#include "ClientQuitException.h"
#include "../common/constants.h"

ClientHandler::ClientHandler(InputParser& _parser): parser(_parser), mouse_was_down(false) {
    initialize_key_map();
    for (auto& [key, state] : key_state) {
        key_state[key] = false;
    }
}

void ClientHandler::handle_event() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            throw ClientQuitException();
        }
        process_event(event);
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q){
            throw ClientQuitException();
        }
    }
    update();
}

void ClientHandler::register_button(const SDL_Rect& rect, ButtonType type) {
    switch (type) {
        case BUTTON_READY:
            parser.parse_command(SEND_READY_TO_PLAY);
            std::cout << "[CLIENT_HANDLER] Ready button clicked - sending R\n";
            break;
        default:
            break;
    }
}

void ClientHandler::process_mouse_click(int x, int y) {
    for (auto& button : buttons) {
        if (button.enabled && 
            x >= button.rect.x && x <= button.rect.x + button.rect.w &&
            y >= button.rect.y && y <= button.rect.y + button.rect.h) {
            handle_button_action(button.type);  
            return;
        }
    }
}

void ClientHandler::process_event(const SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        if (!mouse_was_down) {
            process_mouse_click(event.button.x, event.button.y);
            mouse_was_down = true;
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
        mouse_was_down = false;
    }
    else if (event.type == SDL_KEYDOWN) {
        SDL_Keymod mod = SDL_GetModState();
        if (mod & KMOD_CTRL) {
            
            switch (event.key.keysym.sym) {
                case SDLK_z:
                    parser.parse_command(SEND_WIN_RACE_CHEAT);
                    break;
                case SDLK_x:
                    parser.parse_command(SEND_RESTORE_LIFE_CHEAT);
                    break;
                case SDLK_c:
                    parser.parse_command(SEND_INFINITE_LIFE_CHEAT);
                    break;
                case SDLK_v:
                    parser.parse_command(SEND_LOSE_RACE_CHEAT);
                    break;
                case SDLK_b:
                    parser.parse_command(SEND_INFINITE_NITRO_CHEAT);
                    break;
                case SDLK_q:
                    parser.parse_command(SEND_DISCONNECT);
                    break;
                default:
                    break;
            }
            return;
        }
        key_state[event.key.keysym.sym] = true;
    }
    else if (event.type == SDL_KEYUP) {
        key_state[event.key.keysym.sym] = false;
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