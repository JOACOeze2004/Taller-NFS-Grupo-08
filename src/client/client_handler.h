#ifndef TALLER_TP_CLIENT_HANDLER_H
#define TALLER_TP_CLIENT_HANDLER_H

#include "input_parser.h"
#include <SDL2/SDL.h>
#include <unordered_map>
#include <functional>
#include <vector>

class GraphicClient;

enum ButtonType {
    BUTTON_READY,
    BUTTON_LIFE_UP, BUTTON_LIFE_DOWN,
    BUTTON_VELOCITY_UP, BUTTON_VELOCITY_DOWN,
    BUTTON_ACCELERATION_UP, BUTTON_ACCELERATION_DOWN,
    BUTTON_HANDLING_UP, BUTTON_HANDLING_DOWN,
    BUTTON_CONTROL_UP, BUTTON_CONTROL_DOWN,
    BUTTON_NITRO_UP, BUTTON_NITRO_DOWN,
    BUTTON_SETTINGS
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
    std::unordered_map<ButtonType, int> button_command_map;
    std::unordered_map<SDL_Keycode, int> cheat_command_map;
    std::vector<ClickableButton> buttons;
    bool mouse_was_down;
    GraphicClient* graphic_client;

public:
    explicit ClientHandler(InputParser& _parser);
    void handle_event();
    ~ClientHandler();

    void register_button(const SDL_Rect& rect, ButtonType type);
    void clear_buttons();
    bool is_mouse_over_button(const SDL_Rect& rect) const;
    void set_graphic_client(GraphicClient* gc) { graphic_client = gc; }

private:
    void process_event(const SDL_Event& event);
    void process_mouse_event(const SDL_Event& event);
    void process_keyboard_event(const SDL_Event& event);
    void process_mouse_click(int x, int y);
    void handle_button_action(ButtonType type);
    void handle_pause_menu_event(const SDL_Event& event);
    void update();
    void initialize_key_map();
    void initialize_button_map();
    void initialize_cheat_map();
    bool is_point_in_rect(int x, int y, const SDL_Rect& rect) const;
};

#endif  // TALLER_TP_CLIENT_HANDLER_H