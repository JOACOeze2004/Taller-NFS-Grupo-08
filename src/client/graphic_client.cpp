#include "graphic_client.h"
#include "config.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

GraphicClient::GraphicClient(const Snapshot& initial_snapshot, ClientHandler* _handler, AudioManager* audio)
    : renderer(nullptr), window(nullptr), player_car_id(-1),
      screen_width(DEFAULT_SCREEN_WIDTH), screen_height(DEFAULT_SCREEN_HEIGHT),
      handler(_handler), ready_sent(false), audio_manager(audio),
      previous_collision(NONE_COLLISION), previous_using_nitro(false),
      previous_checkpoint_count(0), human_count(0) {

    initialize_sdl();
    initialize_window();
    initialize_renderer();
    initialize_image_and_ttf();
    initialize_components(initial_snapshot);

    player_car_id = initial_snapshot.id;
    update_from_snapshot(initial_snapshot);
    draw(initial_snapshot);
}

void GraphicClient::initialize_sdl() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error(std::string("[CLIENT] Error initializing SDL: ") + SDL_GetError());
    }
}

void GraphicClient::initialize_window() {
    window = SDL_CreateWindow("Client Game",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              screen_width, screen_height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED);

    if (!window) {
        SDL_Quit();
        throw std::runtime_error(std::string("[CLIENT] Error creating window: ") + SDL_GetError());
    }
}

void GraphicClient::initialize_renderer() {
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error(std::string("[CLIENT] Error creating renderer: ") + SDL_GetError());
    }
}

void GraphicClient::initialize_image_and_ttf() {
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "[CLIENT] Warning: SDL_image PNG support: " << IMG_GetError() << std::endl;
    }

    if (TTF_Init() == -1) {
        throw std::runtime_error(std::string("[CLIENT] Error initializing SDL_ttf: ") + TTF_GetError());
    }
}

void GraphicClient::initialize_components(const Snapshot& initial_snapshot) {
    resources = std::make_unique<ResourceManager>(renderer);

    game_renderer = std::make_unique<GameRenderer>(renderer, resources.get(),
                                                    screen_width, screen_height, ZOOM_FACTOR);
    game_renderer->load_map_texture(initial_snapshot.map);
    game_renderer->load_sprites_config("../src/client/checkpoints_hints.yaml");

    camera_manager = std::make_unique<CameraManager>(screen_width, screen_height,
                                                      game_renderer->get_map_width(),
                                                      game_renderer->get_map_height(),
                                                      ZOOM_FACTOR);
    camera_manager->set_camera_id(initial_snapshot.id);

    ui_renderer = std::make_unique<UIRenderer>(renderer, DEFAULT_FONT_PATH, DEFAULT_FONT_SIZE,
                                                handler, screen_width, screen_height);
    if (!ui_renderer->is_text_ok()) {
        std::cerr << "[CLIENT] Font not available, text won't be rendered." << std::endl;
    }

    minimap_renderer = std::make_unique<MinimapRenderer>(renderer, screen_width, screen_height,
                                                          game_renderer->get_map_width(),
                                                          game_renderer->get_map_height());
    minimap_renderer->set_textures(game_renderer->get_bg_texture(), game_renderer->get_hint_texture());
    minimap_renderer->set_player_car_id(initial_snapshot.id);

    upgrade_phase = std::make_unique<UpgradePhase>(renderer, window, screen_width, screen_height,
                                                    handler, resources.get());
}

void GraphicClient::update_from_snapshot(const Snapshot& snapshot) {
    human_count = 0;
    for (auto& [id, car] : snapshot.cars) {
        update_car(id, car);
        if (car.state != NPC_STATE) ++human_count;
    }

    camera_manager->update( cars);
    clear_cars(snapshot.cars);

    auto player_it = snapshot.cars.find(player_car_id);
    if (player_it != snapshot.cars.end() && player_it->second.state != IN_GAME) {
        for (const auto& [id, car_state] : snapshot.cars) {
            if (car_state.state == IN_GAME && id != player_car_id) {
               camera_manager->set_camera_id(id);
               break;
            }
        }
    } else {
        camera_manager->set_camera_id(player_car_id);
    }

    handle_audio_events(snapshot);
}

void GraphicClient::update_car(int id, const CarDTO& car_state) {
    cars[id] = car_state;
    game_renderer->update_car_objects(id, car_state);
}

void GraphicClient::clear_cars(const std::unordered_map<int, CarDTO>& cars_in_dto) {
    for (auto it = cars.begin(); it != cars.end(); ) {
        if (cars_in_dto.find(it->first) == cars_in_dto.end()) {
            it = cars.erase(it);
        } else {
            ++it;
        }
    }
    game_renderer->clear_car_objects(cars_in_dto);
}

void GraphicClient::handle_audio_events(const Snapshot& snapshot) {
    if (!audio_manager) return;

    auto player_it = snapshot.cars.find(player_car_id);
    if (player_it == snapshot.cars.end()) return;

    const CarDTO& player_car = player_it->second;

    static int previous_state = snapshot.state;
    if (previous_state == IN_LOBBY && snapshot.state == IN_RACE) {
        audio_manager->playSoundEffect(SoundEffect::RACE_START);
    }
    previous_state = snapshot.state;

    if (snapshot.collision != NONE_COLLISION && previous_collision == NONE_COLLISION) {
        if (snapshot.collision == HEAVY_COLLISION) {
            audio_manager->playSoundEffect(SoundEffect::CRASH);
        }
    }
    previous_collision = snapshot.collision;

    bool using_nitro = player_car.remaining_nitro > 0.0f && player_car.nitro > 0.0f;
    if (using_nitro && !previous_using_nitro) {
        audio_manager->playSoundEffect(SoundEffect::NITRO);
    }
    previous_using_nitro = using_nitro;

    if (player_car.state == DEAD) {
        audio_manager->playSoundEffect(SoundEffect::DEATH);
    }

    if (player_car.state == FINISHED && snapshot.state == IN_RACE) {
        audio_manager->playSoundEffect(SoundEffect::WIN);
    }
}

void GraphicClient::draw(const Snapshot& snapshot) {
    switch (snapshot.state) {
        case IN_WORK_SHOP:
            draw_workshop_state(snapshot);
            break;
        case IN_LOBBY:
            draw_lobby_state(snapshot);
            break;
        case IN_RACE:
            draw_race_state(snapshot);
            break;
        default:
            break;
    }
    SDL_RenderPresent(renderer);
}

void GraphicClient::draw_workshop_state(const Snapshot& snapshot) {
    upgrade_phase->render(snapshot);
}

void GraphicClient::draw_lobby_state(const Snapshot& snapshot) {
    float camera_x = camera_manager->get_camera_x();
    float camera_y = camera_manager->get_camera_y();

    game_renderer->draw_background(camera_x, camera_y);
    game_renderer->draw_cars(cars, camera_x, camera_y);
    minimap_renderer->draw(cars, camera_manager->get_camera_id(),
                          snapshot.checkpoint, snapshot.type_checkpoint, snapshot.hint);
    game_renderer->draw_speedometer(cars[player_car_id].velocity);
    ui_renderer->draw_game_id(snapshot.game_id);

    if (snapshot.is_owner) {
        ui_renderer->draw_ready_btn(human_count, ready_sent);
    }
}

void GraphicClient::draw_race_state(const Snapshot& snapshot) {
    float camera_x = camera_manager->get_camera_x();
    float camera_y = camera_manager->get_camera_y();

    game_renderer->draw_background(camera_x, camera_y);
    game_renderer->draw_checkpoint(snapshot.checkpoint, snapshot.type_checkpoint, camera_x, camera_y);
    game_renderer->draw_hint(snapshot.hint, camera_x, camera_y);
    minimap_renderer->draw(cars, camera_manager->get_camera_id(),
                          snapshot.checkpoint, snapshot.type_checkpoint, snapshot.hint);
    game_renderer->draw_speedometer(cars[player_car_id].velocity);
    ui_renderer->draw_game_id(snapshot.game_id);
    game_renderer->draw_cars(cars, camera_x, camera_y);

    auto player_it = snapshot.cars.find(player_car_id);
    if (player_it != snapshot.cars.end() && player_it->second.state != IN_GAME) {
        ui_renderer->draw_state(player_it->second.state);
        ui_renderer->draw_results(snapshot.cars_finished);
    } else {
        ui_renderer->draw_position(snapshot.position, human_count);
        ui_renderer->draw_time(snapshot.time_ms);
    }
}

GraphicClient::~GraphicClient() {
    upgrade_phase.reset();
    minimap_renderer.reset();
    game_renderer.reset();
    ui_renderer.reset();
    camera_manager.reset();
    resources.reset();

    if (TTF_WasInit()) {
        TTF_Quit();
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    IMG_Quit();
    SDL_Quit();
}