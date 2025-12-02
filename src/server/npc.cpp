#include "npc.h"

#include <algorithm>
#include <numeric>
#include "../common/constants.h"

NPC::NPC(b2WorldId world, std::vector<GraphNode>* _corners, int start_corner) : corners(_corners), current_corner(start_corner), world(world), car((*corners)[start_corner], world) {
    next_corner = UNDEFINED_CORNER;
}

void NPC::update() {
    if (corners->empty())
        return;

    reverse_timer_update();

    if (is_stuck()) {
        state = REVERSE;
    }

    if (state == REVERSE) {
        reverse();
        return;
    }

    move();
}

void NPC::move() {
    b2Vec2 pos = car.get_position();

    if (next_corner == UNDEFINED_CORNER)
        choose_next_corner();

    GraphNode target = (*corners)[next_corner];

    float dx = target.x - pos.x;
    float dy = target.y - pos.y;
    float dist2 = dx * dx + dy * dy;

    if (dist2 < MIN_DISTANCE) {
        current_corner = next_corner;
        choose_next_corner();
    }

    car.move(target);
}

void NPC::reverse() {
    reverse_timer -= dt;

    car.reverse();

    if (reverse_timer <= MIN_REVERSE_TIMER) {
        state = NORMAL;
        reverse_timer = MIN_REVERSE_TIMER;
    }
}

void NPC::reverse_timer_update() {
    if (reverse_timer < MAX_REVERSE_TIMER && state == NORMAL) {
        reverse_timer += dt;

        if (reverse_timer >= MAX_REVERSE_TIMER) {
            reverse_timer = MAX_REVERSE_TIMER;
        }
    }
}

bool NPC::is_stuck() {
    float speed = car.get_speed();
    b2Vec2 act_pos = car.get_position();
    b2Vec2 last_pos = car.get_last_pos();
    b2Vec2 pos_dif = {abs(act_pos.x - last_pos.x), abs(act_pos.y - last_pos.y)};

    if (state == 0 && (speed < MIN_SPEED || (pos_dif.x < MIN_DIFF && pos_dif.y < MIN_DIFF)) && reverse_timer == MAX_REVERSE_TIMER) {
        return true;
    }

    return false;
}

void NPC::choose_next_corner() {
    const GraphNode& act_corner = (*corners)[current_corner];
    if (act_corner.neighbors.empty()) {
        next_corner = current_corner;
        return;
    }

    std::vector<int> filtered;
    filter_corners(filtered, act_corner);

    if (filtered.empty()) {
        next_corner = current_corner;
        return;
    }

    std::sort(filtered.begin(), filtered.end(), [&](int a, int b) {
        return act_corner.dist[a] < act_corner.dist[b] && (act_corner.dist[a] - act_corner.dist[b]) > 50;
    });

    int limit = std::min(3, (int)filtered.size());
    if (limit <= 0) {
        next_corner = current_corner;
        return;
    }
    next_corner = act_corner.neighbors[ filtered[rand() % limit] ];
}

void NPC::filter_corners(std::vector<int>& filtered, const GraphNode& act_corner) {
    std::vector<int> idx(act_corner.neighbors.size());
    std::iota(idx.begin(), idx.end(), 0);
    b2Vec2 pos = car.get_position();
    b2Vec2 forward = car.get_forward();

    for (int i : idx) {
        int nb = act_corner.neighbors[i];
        const auto& corner = (*corners)[nb];

        b2Vec2 to_corner = { corner.x - pos.x, corner.y - pos.y };

        float dot = forward.x * to_corner.x + forward.y * to_corner.y;

        if (dot > 0.0f) {
            filtered.push_back(i);
        }
    }
}

CarDTO NPC::get_state() {
    return car.get_state();
}
