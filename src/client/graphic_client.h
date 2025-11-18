#ifndef TALLER_TP_GRAPHIC_CLIENT_H
#define TALLER_TP_GRAPHIC_CLIENT_H
#include <SDL2/SDL.h>
#include <string>
#include <memory>
#include <unordered_map>
#include "client_car.h"
#include "../common/DTO.h"
#include "text_renderer.h"
#include "client_handler.h"
#include "upgrade_phase.h"

constexpr int DEFAULT_SCREEN_WIDTH = 1200;
constexpr int DEFAULT_SCREEN_HEIGHT = 900;
constexpr float ZOOM_FACTOR = 2.0f;
constexpr int DEFAULT_FONT_SIZE = 20;
constexpr const char* DEFAULT_FONT_PATH = "../assets/fonts/DejaVuSans.ttf";
constexpr const char* CHECKPOINT_TEXTURE_PATH = "../assets/need-for-speed/sprits/checkpoints.png";
constexpr const char* HINT_TEXTURE_PATH = "../assets/need-for-speed/sprits/Hints.png";
constexpr const char* SPEEDOMETER_TEXTURE_PATH = "../assets/need-for-speed/sprits/Speedmeter.png";
constexpr const char* LIBERTY_CITY_MAP_PATH = "../assets/need-for-speed/cities/Game Boy _ GBC - Grand Theft Auto - Backgrounds - Liberty City.png";
constexpr const char* SAN_ANDREAS_MAP_PATH = "../assets/need-for-speed/cities/Game Boy _ GBC - Grand Theft Auto - Backgrounds - San Andreas.png";
constexpr const char* VICE_CITY_MAP_PATH = "../assets/need-for-speed/cities/Game Boy _ GBC - Grand Theft Auto - Backgrounds - Vice City.png";

class GraphicClient {
    SDL_Renderer* renderer;
    SDL_Texture* bg_texture;
    SDL_Texture* checkpoint_texture;
    SDL_Texture* hint_texture;
    SDL_Texture* speedometer_texture;
    SDL_Window* window;
    std::unordered_map<int, CarDTO> cars;
    std::unordered_map<int, std::unique_ptr<Car>> car_objects;

    int player_car_id;
    float camera_x, camera_y;
    int screen_width, screen_height;
    float map_width, map_height;
    std::unique_ptr<TextRenderer> text;

    int camera_id;
    ClientHandler* handler;
    bool ready_sent;
    std::unique_ptr<UpgradePhase> upgrade_phase;

    public:
    explicit GraphicClient(const Snapshot& initial_snapshot, ClientHandler* _handler);
    void draw(const Snapshot& snapshot);
    void update_from_snapshot(const Snapshot& snapshot);
    ~GraphicClient();
    
    private:
    void clear_cars(const std::unordered_map<int, CarDTO>& cars_in_dto);
    void draw_cars();
    void draw_camera();
    void draw_minimap(const CheckpointCoords& checkpoint, int checkpoint_type, const HintCoords& hint);
    void update_camera();
    void update_car(int id, const CarDTO& car_state);
    void set_player_car(int id);
    void draw_position(int position, int total_cars);
    void draw_time(int time_ms);
    void draw_speedometer(float speed);
    void draw_life(int life);
    void draw_checkpoint(CheckpointCoords checkpoint, int type);
    void draw_hint(HintCoords hint);
    void draw_game_id(int id);
    void draw_state(int state);
    void draw_ready_btn(int player_count, bool& ready_sent);
    void draw_results(const std::vector<CarRacingInfo>& cars_finished);
};


#endif  // TALLER_TP_GRAPHIC_CLIENT_H
