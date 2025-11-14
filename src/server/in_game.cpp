#include "in_game.h"
#include "gameloop.h" 

InGame::InGame(Gameloop* _gameloop, float _duration, std::string& map_name, const std::string& races_path, std::map<int, Car>* cars): Phase(_gameloop, _duration), current_map(map_name), race(races_path, cars), cars(cars) {}

void InGame::set_start_race() {
    start_time = std::chrono::steady_clock::now();
    Checkpoint start = race.get_start();
    float start_angle = race.get_start_angle();
    for (auto& [id, car] : *cars) {
        car.set_spawn(start.x,start.y, start_angle);
    }
    update_positions();
    gameloop->broadcast_in_game();
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

void InGame::run() {
    set_start_race();

    auto rate = std::chrono::milliseconds(16);
    auto t1 = std::chrono::steady_clock::now();

    while (should_continue()) {
        execute();
        update_positions();

        gameloop->broadcast_in_game();

        check_end_race();

        auto t2 = std::chrono::steady_clock::now();
        auto rest = rate - std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
        if (rest < std::chrono::milliseconds(0)) {
            auto behind = -rest;
            rest = rate - behind % rate;
            auto lost = behind + rest;
            t1 += lost;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(rest));
        t1 += rate;
    }
}

void InGame::check_end_race() {
    int cars_finished = 0;
    for (auto& [id, car] : *cars) {
        if (race.car_finished(id)) {
            cars_finished++;
        }
    }

    if (cars_finished == static_cast<int>(cars->size())) {
        cont = false;
    }
}

void InGame::execute() {
    ClientCommand command{};
    while (gameloop->try_pop(command)) {
        gameloop->process_command(command);
    }
    update_positions();
}

int InGame::get_time_remaining_ms() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
    return std::max(0, MAX_TIME_PER_RACE - static_cast<int>(elapsed));
}

StateRunning InGame::get_state(const int& id) {
    const StateRunning state = race.get_state(id, get_time_remaining_ms());
    return state;
}

CheckpointCoords InGame::get_checkpoint(const int& id) const {
    return race.get_checkpoint(id);
}

HintCoords InGame::get_hint(const int& id) const {
    return race.get_hint(id);
}
TypeCheckpoint InGame::get_cp_type(const int& id) const {
    return race.get_cp_type(id);
}

int InGame::get_position(const int& id) const {
    return race.get_position(id);
}

void InGame::update_positions() {
    gameloop->update_positions();
    race.update_checkpoints();
    race.update_positions_order();
    for (auto& [id, car] : *cars) {
        car.update_nitro_usage();
        car.apply_nitro_force();
    }
}

void InGame::delete_deads() {
    for (auto& [id, car] : *cars) {
        if (car.is_dead() || race.car_finished(id)) {
            car.delete_from_map();
        }
    }
}

