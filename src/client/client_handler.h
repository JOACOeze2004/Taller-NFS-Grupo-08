#ifndef TALLER_TP_CLIENT_HANDLER_H
#define TALLER_TP_CLIENT_HANDLER_H

#include "input_parser.h"
#include <SDL2/SDL.h>
#include <unordered_map>
#include <functional>

class ClientHandler {
    InputParser& parser;
    std::unordered_map<SDL_Keycode, std::function<void()>> key_map;
    std::unordered_map<SDL_Keycode, bool> key_state;

public:
    explicit ClientHandler(InputParser& _parser);
    void handle_event();

private:
    void process_event(const SDL_Event& event);
    void update();
    void initialize_key_map();

};

#endif  // TALLER_TP_CLIENT_HANDLER_H
