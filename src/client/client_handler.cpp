#include "client_handler.h"
#include "ClientQuitException.h"
#include "../common/constants.h"

ClientHandler::ClientHandler(InputParser& _parser): parser(_parser) {
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
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q){
            throw ClientQuitException();
        }
        process_event(event);
        }
        update();
    }

void ClientHandler::initialize_key_map() {
    key_map[SDLK_w] = [this]() { parser.parse_command(SEND_ACCELERATE); };
    key_map[SDLK_a] = [this]() { parser.parse_command(SEND_ROTATE_LEFT); };
    key_map[SDLK_s] = [this]() { parser.parse_command(SEND_BRAKE); };
    key_map[SDLK_d] = [this]() { parser.parse_command(SEND_ROTATE_RIGHT); };
}

void ClientHandler::process_event(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
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
                default:
                    break;
            }
                return; //ver si apretar igual el ctrl wasdn o que no se tome por apretar el ctl
        }
        key_state[event.key.keysym.sym] = true;
    }
    else if (event.type == SDL_KEYUP) {
        key_state[event.key.keysym.sym] = false;
    }
}

void ClientHandler::update() {
    for (auto& [key, func] : key_map) {
        if (key_state[key]) {func();}
    }
}