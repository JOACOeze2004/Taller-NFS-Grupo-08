#ifndef TALLER_TP_CLIENT_HANDLER_H
#define TALLER_TP_CLIENT_HANDLER_H

#include "input_parser.h"
#include <SDL2/SDL.h>
#include <unordered_map>
#include <functional>

class ClientHandler {
    InputParser& parser;

public:
    explicit ClientHandler(InputParser& _parser);
    void handle_event();
};

class KeyboardHandler {
public:
    explicit KeyboardHandler(InputParser& _parser);
    void process_event(const SDL_Event& event);
    void update();

private:
    void initialize_key_map();
    
    InputParser& parser;
    std::unordered_map<SDL_Keycode, std::function<void()>> key_map;
    std::unordered_map<SDL_Keycode, bool> key_state;
};

#endif  // TALLER_TP_CLIENT_HANDLER_H
