#include "npc.h"

#include <algorithm>
#include <numeric>

NPC::NPC(b2WorldId world, std::vector<GraphNode>* _corners, int start_corner) : corners(_corners), current_corner(start_corner), world(world), car((*corners)[start_corner], world) {
    next_corner = -1;
}

void NPC::update() {
    if (corners->empty())
        return;

    float speed = car.get_speed();
    float dt = 1.0f / 60.0f;
    if (reverse_timer < 1.0f && state == 0) {
        reverse_timer += dt;
        if (reverse_timer >= 1.0f) {
            reverse_timer = 1.0f;
        }
    }

    b2Vec2 act_pos = car.get_position();
    b2Vec2 last_pos = car.get_last_pos();
    b2Vec2 pos_dif = {abs(act_pos.x - last_pos.x), abs(act_pos.y - last_pos.y)};

    if (state == 0 && (speed < 0.25 || (pos_dif.x < 0.01 && pos_dif.y < 0.01)) && reverse_timer == 1.0f) {
        state = 1;
    }

    if (state == 1) {
        reverse_timer -= dt;

        car.reverse();

        if (reverse_timer <= 0.0f) {
            state = 0;
            reverse_timer = 0;
        }
        return;
    }

    b2Vec2 pos = car.get_position();

    if (next_corner == -1)
        choose_next_corner();

    GraphNode target = (*corners)[next_corner];

    float dx = target.x - pos.x;
    float dy = target.y - pos.y;
    float dist2 = dx * dx + dy * dy;

    if (dist2 < 100.0f) {
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
