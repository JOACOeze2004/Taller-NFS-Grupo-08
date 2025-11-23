//
// Created by vboxuser on 11/21/25.
//

#include "npc.h"

#include <algorithm>
#include <numeric>

NPC::NPC(b2WorldId world, std::vector<GraphNode>* _corners, int start_corner) : corners(_corners), current_corner(start_corner), world(world), car((*corners)[start_corner], world) {
    next_corner = -1;
}

void NPC::update() {
    if (corners->empty()) {
        return;
    }

    b2Vec2 car_pos = car.get_position();

    if (next_corner == -1) {
        choose_next_corner();
    }

    GraphNode target = (*corners)[next_corner];

    float dx = target.x - car_pos.x;
    float dy = target.y - car_pos.y;
    float dist = dx * dx + dy * dy;

    if (dist < 2.0f) {
        current_corner = next_corner;
        choose_next_corner();
    }

    car.move(target);
}

void NPC::choose_next_corner() {
    const auto& corner = (*corners)[current_corner];

    if (corner.neighbors.empty()) {
        next_corner = 4;  // maaaaal la mayoria estan vacias por eso pongo esto.
        return;
    };

    std::vector<int> idx(corner.neighbors.size());
    std::iota(idx.begin(), idx.end(), 0);

    std::sort(idx.begin(), idx.end(), [&]( int& a, const int& b) {
        return corner.dist[a] < corner.dist[b];
    });

    const int limit = std::min(3, static_cast<int>(idx.size()));
    const int next = idx[rand() % limit];

    next_corner = corner.neighbors[next];
}

CarDTO NPC::get_state() {
    return car.get_state();
}
