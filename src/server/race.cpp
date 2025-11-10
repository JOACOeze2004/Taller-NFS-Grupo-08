#include "race.h"

#include <iostream>

Race::Race(const std::string& track_name, std::map<int, Car> *_cars) : track(parser.parse_tracks(track_name)), cars(_cars) {
    for (auto& [id, car] : *cars) {
        finished[id] = false;
        car_progress[id] = 0;
    }
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

