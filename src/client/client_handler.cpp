#include "client_handler.h"
#include "client_handler.h"
#include "../common/constants.h"

// ClientHandler implementation
ClientHandler::ClientHandler(InputParser& _parser): parser(_parser) {}

void ClientHandler::handle_event() {
    // TODO: Implementar si es necesario
}

// KeyboardHandler implementation
KeyboardHandler::KeyboardHandler(InputParser& _parser): parser(_parser) {
    initialize_key_map();
}

void KeyboardHandler::initialize_key_map() {
    key_map[SDLK_w] = [this]() { parser.parse_command(SEND_ACCELERATE); };
    key_map[SDLK_a] = [this]() { parser.parse_command(SEND_ROTATE_LEFT); };
    key_map[SDLK_s] = [this]() { parser.parse_command(SEND_BRAKE); };
    key_map[SDLK_d] = [this]() { parser.parse_command(SEND_ROTATE_RIGHT); };
}

void KeyboardHandler::process_event(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        auto it = key_map.find(event.key.keysym.sym);
        if (it != key_map.end()) {
            it->second();
        }
    }
}