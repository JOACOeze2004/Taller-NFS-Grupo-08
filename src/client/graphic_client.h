#ifndef TALLER_TP_GRAPHIC_CLIENT_H
#define TALLER_TP_GRAPHIC_CLIENT_H

#include <SDL2/SDL.h>
#include <memory>
#include <unordered_map>
#include "../common/DTO.h"
#include "camera_manager.h"
#include "ui_renderer.h"
#include "game_renderer.h"
#include "minimap_renderer.h"
#include "upgrade_phase.h"
#include "resource_manager.h"
#include "audio_manager.h"
#include "sprite_loader.h"
#include "config.h"

class GraphicClient {
private:
    SDL_Renderer* renderer;
    SDL_Window* window;

    std::unique_ptr<ResourceManager> resources;
    std::unique_ptr<CameraManager> camera_manager;
    std::unique_ptr<UIRenderer> ui_renderer;
    std::unique_ptr<GameRenderer> game_renderer;
    std::unique_ptr<MinimapRenderer> minimap_renderer;
    std::unique_ptr<UpgradePhase> upgrade_phase;

    std::unordered_map<int, CarDTO> cars;
    int player_car_id;
    int screen_width, screen_height;

    ClientHandler* handler;
    bool ready_sent;
    AudioManager* audio_manager;

    SDL_Texture* upgrade_icons_texture;
    std::vector<UpgradeData> upgrade_sprites;

    CollisionType previous_collision;
    bool previous_using_nitro;
    int previous_checkpoint_count;
    int human_count;

public:
    explicit GraphicClient(const Snapshot& initial_snapshot, ClientHandler* _handler, AudioManager* audio);
    ~GraphicClient();

    void draw(const Snapshot& snapshot);
    void update_from_snapshot(const Snapshot& snapshot);

private:
    void initialize_sdl();
    void initialize_window();
    void initialize_renderer();
    void initialize_image_and_ttf();
    void initialize_components(const Snapshot& initial_snapshot);

    void draw_workshop_state(const Snapshot& snapshot);
    void draw_lobby_state(const Snapshot& snapshot);
    void draw_race_state(const Snapshot& snapshot);

    void update_car(int id, const CarDTO& car_state);
    void clear_cars(const std::unordered_map<int, CarDTO>& cars_in_dto);
    void handle_audio_events(const Snapshot& snapshot);
};

#endif // TALLER_TP_GRAPHIC_CLIENT_H