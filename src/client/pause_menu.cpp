#include "pause_menu.h"
#include "config.h"
#include <algorithm>

PauseMenu::PauseMenu(SDL_Renderer* rend, const std::string& font_path, int font_size,
                     AudioManager* audio, int screen_w, int screen_h)
    : renderer(rend), audio_manager(audio), 
      screen_width(screen_w), screen_height(screen_h),
      music_volume(30), effects_volume(30),
      dragging_music(false), dragging_effects(false) {
    
    text = std::make_unique<TextRenderer>(font_path, font_size);

    const int slider_width = 300;
    const int slider_height = 10;
    const int center_x = screen_width / 2;
    
    music_slider_track = {center_x - slider_width/2, 200, slider_width, slider_height};
    effects_slider_track = {center_x - slider_width/2, 280, slider_width, slider_height};
    
    const int button_width = 200;
    const int button_height = 50;
    const int button_spacing = 20;
    
    resume_button = {center_x - button_width/2, 380, button_width, button_height};
    quit_button = {center_x - button_width/2, 380 + button_height + button_spacing, 
                   button_width, button_height};

    if (audio_manager) {
        music_volume = 30;
        effects_volume = 30;
    }
}

void PauseMenu::render() {
    draw_overlay();
    draw_title();

    draw_slider(music_slider_track, music_volume, "MUSIC VOLUME", 170);
    draw_slider(effects_slider_track, effects_volume, "EFFECTS VOLUME", 250);

    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    
    bool resume_hovered = is_point_in_rect(mouse_x, mouse_y, resume_button);
    bool quit_hovered = is_point_in_rect(mouse_x, mouse_y, quit_button);
    
    draw_button(resume_button, "RESUME", resume_hovered);
    draw_button(quit_button, "QUIT", quit_hovered);
}

void PauseMenu::draw_overlay() {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect fullscreen = {0, 0, screen_width, screen_height};
    SDL_RenderFillRect(renderer, &fullscreen);
}

void PauseMenu::draw_title() {
    if (!text) return;
    
    SDL_Color title_color = {255, 215, 0, 255};
    std::string title = "PAUSE MENU";
    int title_width = static_cast<int>(title.size()) * 12;
    int title_x = (screen_width - title_width) / 2;
    text->render(renderer, title, title_x, 80, title_color);
}

void PauseMenu::draw_slider(const SDL_Rect& track, int volume, const std::string& label, int label_y) {
    if (!text) return;

    SDL_Color label_color = {255, 255, 255, 255};
    int label_width = static_cast<int>(label.size()) * 10;
    int label_x = track.x + (track.w - label_width) / 2;
    text->render(renderer, label, label_x, label_y, label_color);

    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(renderer, &track);

    SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
    SDL_RenderDrawRect(renderer, &track);

    int filled_width = (volume * track.w) / 128;
    SDL_Rect filled = {track.x, track.y, filled_width, track.h};
    SDL_SetRenderDrawColor(renderer, 30, 144, 255, 255);
    SDL_RenderFillRect(renderer, &filled);

    SDL_Rect handle = get_slider_handle(track, volume);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &handle);

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &handle);

    int percentage = (volume * 100) / 128;
    std::string volume_text = std::to_string(percentage) + "%";
    int volume_x = track.x + track.w + 20;
    int volume_y = track.y - 5;
    text->render(renderer, volume_text, volume_x, volume_y, label_color);
}

void PauseMenu::draw_button(const SDL_Rect& button, const std::string& label, bool is_hovered) {
    if (!text) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    if (label == "QUIT") {
        if (is_hovered) {
            SDL_SetRenderDrawColor(renderer, 220, 40, 40, 230);
        } else {
            SDL_SetRenderDrawColor(renderer, 180, 30, 30, 200);
        }
    } else {
        if (is_hovered) {
            SDL_SetRenderDrawColor(renderer, 50, 164, 255, 230);
        } else {
            SDL_SetRenderDrawColor(renderer, 30, 144, 255, 200);
        }
    }

    SDL_RenderFillRect(renderer, &button);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &button);
    SDL_Color label_color = {255, 255, 255, 255};
    int char_width_estimate = 16;
    int label_width = static_cast<int>(label.size()) * char_width_estimate;
    int label_x = button.x + (button.w - label_width) / 2;
    int label_y = button.y + (button.h - 25) / 2;

    text->render(renderer, label, label_x, label_y, label_color);
}

SDL_Rect PauseMenu::get_slider_handle(const SDL_Rect& track, int volume) {
    const int handle_width = 20;
    const int handle_height = 20;
    int handle_x = track.x + (volume * track.w) / 128 - handle_width / 2;
    int handle_y = track.y + track.h / 2 - handle_height / 2;
    return {handle_x, handle_y, handle_width, handle_height};
}

bool PauseMenu::is_point_in_rect(int x, int y, const SDL_Rect& rect) {
    return x >= rect.x && x <= rect.x + rect.w &&
           y >= rect.y && y <= rect.y + rect.h;
}

int PauseMenu::calculate_volume_from_mouse(int mouse_x, const SDL_Rect& track) {
    int relative_x = mouse_x - track.x;
    relative_x = std::max(0, std::min(track.w, relative_x));
    return (relative_x * 128) / track.w;
}

PauseMenuAction PauseMenu::handle_click(int mouse_x, int mouse_y) {
    if (is_point_in_rect(mouse_x, mouse_y, resume_button)) {
        return PAUSE_RESUME;
    }
    if (is_point_in_rect(mouse_x, mouse_y, quit_button)) {
        return PAUSE_QUIT;
    }
    return PAUSE_NONE;
}

void PauseMenu::handle_mouse_down(int mouse_x, int mouse_y) {
    SDL_Rect music_handle = get_slider_handle(music_slider_track, music_volume);
    if (is_point_in_rect(mouse_x, mouse_y, music_handle) || 
        is_point_in_rect(mouse_x, mouse_y, music_slider_track)) {
        dragging_music = true;
        music_volume = calculate_volume_from_mouse(mouse_x, music_slider_track);
        if (audio_manager) {
            audio_manager->setMusicVolume(music_volume);
        }
    }

    SDL_Rect effects_handle = get_slider_handle(effects_slider_track, effects_volume);
    if (is_point_in_rect(mouse_x, mouse_y, effects_handle) || 
        is_point_in_rect(mouse_x, mouse_y, effects_slider_track)) {
        dragging_effects = true;
        effects_volume = calculate_volume_from_mouse(mouse_x, effects_slider_track);
        if (audio_manager) {
            audio_manager->setEffectsVolume(effects_volume);
        }
    }
}

void PauseMenu::handle_mouse_up() {
    dragging_music = false;
    dragging_effects = false;
}

void PauseMenu::handle_mouse_motion(int mouse_x) {
    if (dragging_music) {
        music_volume = calculate_volume_from_mouse(mouse_x, music_slider_track);
        if (audio_manager) {
            audio_manager->setMusicVolume(music_volume);
        }
    }
    
    if (dragging_effects) {
        effects_volume = calculate_volume_from_mouse(mouse_x, effects_slider_track);
        if (audio_manager) {
            audio_manager->setEffectsVolume(effects_volume);
        }
    }
}