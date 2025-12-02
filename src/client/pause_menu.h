#ifndef TALLER_PAUSE_MENU_H
#define TALLER_PAUSE_MENU_H

#include <SDL2/SDL.h>
#include <memory>
#include "text_renderer.h"
#include "audio_manager.h"

enum PauseMenuAction {
    PAUSE_NONE,
    PAUSE_RESUME,
    PAUSE_QUIT
};

class GraphicClient;

class PauseMenu {
private:
    SDL_Renderer* renderer;
    std::unique_ptr<TextRenderer> text;
    AudioManager* audio_manager;
    int screen_width;
    int screen_height;

    int music_volume;
    int effects_volume;
    bool dragging_music;
    bool dragging_effects;

    SDL_Rect music_slider_track;
    SDL_Rect effects_slider_track;
    SDL_Rect resume_button;
    SDL_Rect quit_button;
    
    void draw_overlay();
    void draw_title();
    void draw_slider(const SDL_Rect& track, int volume, const std::string& label, int label_y);
    void draw_button(const SDL_Rect& button, const std::string& label, bool is_hovered);
    SDL_Rect get_slider_handle(const SDL_Rect& track, int volume);
    bool is_point_in_rect(int x, int y, const SDL_Rect& rect);
    int calculate_volume_from_mouse(int mouse_x, const SDL_Rect& track);

public:
    PauseMenu(SDL_Renderer* rend, const std::string& font_path, int font_size,
              AudioManager* audio, int screen_w, int screen_h);
    
    void render();
    PauseMenuAction handle_click(int mouse_x, int mouse_y);
    void handle_mouse_down(int mouse_x, int mouse_y);
    void handle_mouse_up();
    void handle_mouse_motion(int mouse_x);
    
    bool is_dragging() const { return dragging_music || dragging_effects; }
};

#endif // TALLER_PAUSE_MENU_H