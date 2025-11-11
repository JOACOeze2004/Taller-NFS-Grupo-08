#include "race.h"

#include <iostream>

Race::Race(const std::string& track_name, std::map<int, Car> *_cars) : track(parser.parse_tracks(track_name)), cars(_cars) {
    for (auto& [id, car] : *cars) {
        finished[id] = false;
        car_progress[id] = 0;
    }
}

int Race::get_checkpoints(const int id) const {
    auto it = car_progress.find(id);
    if (it == car_progress.end()){
        return 0;
    } 
    return it->second;
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
        int next_checkpoint = car_progress[id];
        if (next_checkpoint >= static_cast<int>(track.checkpoints.size())) continue;

        const auto& cp = track.checkpoints[next_checkpoint];
        auto [x, y] = car.get_position();

        float dx = x - cp.x;
        float dy = y - cp.y;
        float dist2 = dx*dx + dy*dy;
        const float radius = 50.0f;

        if (dist2 <= radius * radius) {
            car_progress[id]++;

            std::cout << "Auto " << id
                      << " pasa por checkpoint " << cp.order
                      << " (" << car_progress[id] << "/" << track.checkpoints.size() << ")" << std::endl;

            if (car_progress[id] == static_cast<int>(track.checkpoints.size())) {
                finished[id] = true;
                std::cout << "Auto " << id << " completo la carrera" << std::endl;
            }
        }
    }
}

bool Race::car_dead(const int& id) {
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

Checkpoint Race::get_checkpoint(const int id) const {
    auto it = car_progress.find(id);
    if (it == car_progress.end()){
        return {0, 0, 0};
    }
    return track.checkpoints[it->second];
}
