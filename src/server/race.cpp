#include "race.h"

#include <iostream>

Race::Race(const std::string& map_name, std::map<int, Car> *_cars, const std::string& tracks_directory) : tracks(parser.parse_tracks(tracks_directory, map_name)), cars(_cars) {
    int indx = rand() % tracks.size();
    track = tracks[indx];
    for (auto& [id, car] : *cars) {
        finished[id] = false;
        car_next_cp[id] = 0;
        car_next_hint[id] = 0;
    }
}

int Race::get_checkpoints(const int id) const {
    auto it = car_next_cp.find(id);
    if (it == car_next_cp.end()){
        return 0;
    } 
    return it->second;
}

TypeCheckpoint Race::get_cp_type(const int& id) const {
    int checkpoint = get_checkpoints(id);
    if (checkpoint == 0) {
        return FIRST;
    }
    if (checkpoint+1 >= static_cast<int>(track.checkpoints.size())) {
        return FINAL;
    }

    return REGULAR;
}

int Race::get_position(const int& id) const {
    for (size_t i = 0; i < positions_order.size(); ++i) {
        if (positions_order[i] == id)
            return static_cast<int>(i) + 1;
    }
    return static_cast<int>(positions_order.size());
}

float Race::get_distance(int id) const {
    int checkpoint = get_checkpoints(id);
    if (checkpoint >= static_cast<int>(track.checkpoints.size())) {
        return std::numeric_limits<float>::max();
    }
    const auto& next_checkpoint = track.checkpoints[checkpoint];
    auto it = cars->find(id);
    if (it == cars->end()) {
        return std::numeric_limits<float>::max();
    }
    auto [x, y] = it->second.get_position();
    return std::sqrt((x - next_checkpoint.x) * (x - next_checkpoint.x) + (y - next_checkpoint.y) * (y - next_checkpoint.y));
}

bool Race::compare_cars(int a, int b) const {
    int checkpoints_a = get_checkpoints(a);
    int checkpoints_b = get_checkpoints(b);

    if (checkpoints_a != checkpoints_b){
        return checkpoints_a > checkpoints_b;
    }

    float dist_a = get_distance(a);
    float dist_b = get_distance(b);
    return dist_a < dist_b;
}

void Race::update_positions_order() {
    std::vector<int> ids;
    ids.reserve(cars->size());
    for (const auto& [id, _] : *cars) {
        ids.push_back(id);
    }

    std::sort(ids.begin(), ids.end(), [this](int a, int b) {
        return compare_cars(a, b);
    });

    positions_order = std::move(ids);
}

void Race::update_checkpoints() {
    for (auto& [id, car] : *cars) {
        if (car_finished(id)) {
            continue;
        }

        int next_checkpoint = car_next_cp[id];
        if (next_checkpoint >= static_cast<int>(track.checkpoints.size())) continue;

        const auto& cp = track.checkpoints[next_checkpoint];
        auto [x, y] = car.get_position();

        float dx = x - cp.x;
        float dy = y - cp.y;
        float dist2 = dx*dx + dy*dy;
        const float radius = 50.0f;

        if (dist2 <= radius * radius) {
            if (next_checkpoint != 0) {
                car_next_hint[id]++;
            }
            car_next_cp[id]++;

            std::cout << "Auto " << id
                      << " pasa por checkpoint " << cp.order
                      << " (" << car_next_cp[id] << "/" << track.checkpoints.size() << ")" << std::endl;

            if (car_next_cp[id] == static_cast<int>(track.checkpoints.size())) {
                finished[id] = true;
                std::cout << "Auto " << id << " completo la carrera" << std::endl;
            }
        }
    }
}

bool Race::car_dead(const int& id) const {
    auto it = cars->find(id);
    return it->second.is_dead();
}

bool Race::car_finished(const int& id) {
    return finished[id];
}

StateRunning Race::get_state(const int& id, const int& time_remaining) {
    if (car_finished(id)) {
        return FINISHED;
    }
    if (car_dead(id)) {
        return DEAD;
    }
    if (time_remaining <= 0) {
        return TIMEOUTED;
    }

    return IN_GAME;
}

CheckpointCoords Race::get_checkpoint(const int& id) const {
    auto it = car_next_cp.find(id);
    if (it == car_next_cp.end()) {
        return {0,0};
    }
    size_t next_cp = it->second;
    if (next_cp >= track.checkpoints.size()) {
        const Checkpoint& last = track.checkpoints.back();
        return {last.x, last.y};
    }
    const Checkpoint& checkpoint = track.checkpoints[next_cp];
    return {checkpoint.x, checkpoint.y};
}

HintCoords Race::get_hint(const int& id) const {
    auto it = car_next_hint.find(id);
    if (it == car_next_hint.end()){
        return {0.f, 0.f, 0.f};
    }
    int next_hint = it->second;

    if (next_hint < 0 || next_hint >= static_cast<int>(track.hints.size())) {
        return {0.f, 0.f, 0.f};
    }

    const auto& [x, y, rotation] = track.hints[next_hint];
    return HintCoords{x, y, rotation};
}

Checkpoint Race::get_start() const {
    return track.checkpoints[0];
}

Checkpoint Race::get_start_angle() const {
    return track.checkpoints[1];
}

void Race::reset_race() {
    std::cout << "[RACE] Resetting race for next round" << std::endl;
    for (auto& [id, car] : *cars) {
        finished[id] = false;
        car_next_cp[id] = 0;
        car_next_hint[id] = 0;
        car.reset_stats_and_upgrades();
    }
    positions_order.clear();
    int indx = rand() % tracks.size();
    track = tracks[indx];
}

void Race::activate_win(int& id) {
    finished[id] = true;
}