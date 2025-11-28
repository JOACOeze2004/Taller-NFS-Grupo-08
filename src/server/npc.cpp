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

    if (dist < 100.0f) {
        current_corner = next_corner;
        choose_next_corner();
    }

    car.move(target);
}

void NPC::choose_next_corner() {
    const auto& corner = (*corners)[current_corner];
    if (corner.neighbors.empty()) {
        next_corner = current_corner;
        return;
    }

    b2Vec2 pos = car.get_position();
    b2Vec2 forward = car.get_forward();

    std::vector<int> idx(corner.neighbors.size());
    std::iota(idx.begin(), idx.end(), 0);

    std::vector<int> filtered;

    for (int i : idx) {
        int nb = corner.neighbors[i];
        const auto& c = (*corners)[nb];

        b2Vec2 toC = { c.x - pos.x, c.y - pos.y };

        float dot = forward.x * toC.x + forward.y * toC.y;

        if (dot > 0.0f) {
            filtered.push_back(i);
        }
    }

    if (filtered.empty()) {
        filtered = idx;
    }
    if (filtered.empty()) {
        next_corner = current_corner;
        return;
    }

    std::sort(filtered.begin(), filtered.end(), [&](int a, int b) {
        return corner.dist[a] < corner.dist[b] && (corner.dist[a] - corner.dist[b]) > 50;
    });

    int limit = std::min(3, (int)filtered.size());
    if (limit <= 0) {
        next_corner = current_corner;
        return;
    }
    next_corner = corner.neighbors[ filtered[rand() % limit] ];
}

CarDTO NPC::get_state() {
    return car.get_state();
}
