#ifndef TALLER_TP_CLIENT_HANDLER_H
#define TALLER_TP_CLIENT_HANDLER_H

#include "input_parser.h"
#include <SDL2/SDL.h>
#include <unordered_map>
#include <functional>
#include <vector>

enum ButtonType {
    BUTTON_READY,
    // Arrow upgrade/downgrade buttons (right = upgrade, left = downgrade)
    BUTTON_LIFE_UP, BUTTON_LIFE_DOWN,
    BUTTON_VELOCITY_UP, BUTTON_VELOCITY_DOWN,
    BUTTON_ACCELERATION_UP, BUTTON_ACCELERATION_DOWN,
    BUTTON_HANDLING_UP, BUTTON_HANDLING_DOWN,
    BUTTON_CONTROL_UP, BUTTON_CONTROL_DOWN,
    BUTTON_NITRO_UP, BUTTON_NITRO_DOWN,
};

struct ClickableButton {
    SDL_Rect rect;
    ButtonType type;
    bool enabled;
    
    ClickableButton(SDL_Rect r, ButtonType t) 
        : rect(r), type(t), enabled(true) {}
};

class ClientHandler {
    InputParser& parser;
    std::unordered_map<SDL_Keycode, std::function<void()>> key_map;
    std::unordered_map<SDL_Keycode, bool> key_state;
    std::vector<ClickableButton> buttons;
    bool mouse_was_down;

public:
    explicit ClientHandler(InputParser& _parser);
    void handle_event();
    ~ClientHandler();
    
    void register_button(const SDL_Rect& rect, ButtonType type);
    void clear_buttons();
    bool is_mouse_over_button(const SDL_Rect& rect) const;

private:
    void process_event(const SDL_Event& event);
    void process_mouse_click(int x, int y);
    void handle_button_action(ButtonType type);  
    void update();
    void initialize_key_map();
};

#endif  // TALLER_TP_CLIENT_HANDLER_H
