//
// Created by vboxuser on 11/21/25.
//

#include "npc.h"

#include <algorithm>

struct Candidate {
    size_t index;
    float dist;
};

NPC::NPC(b2WorldId world, std::vector<Corner>* _corners, int start_corner) : corners(_corners), current_corner(start_corner), world(world), car((*corners)[start_corner], world) {
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

    Corner target = (*corners)[next_corner];

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
    std::vector<Candidate> visible_corners;

    for (size_t i = 0; i < corners->size(); i++) {
        if (i == current_corner) {
            continue;
        }

        if (is_visible((*corners)[i], (*corners)[current_corner])) {
            const float dx = (*corners)[i].x - (*corners)[current_corner].x;
            const float dy = (*corners)[i].y - (*corners)[current_corner].y;
            const float dist = dx*dx + dy*dy;
            visible_corners.push_back({i, dist});
        }
    }

    if (visible_corners.empty()) {
        return;
    }

    std::sort(visible_corners.begin(), visible_corners.end(), [](const Candidate& a, const Candidate& b) {
        return a.dist < b.dist;
    });

    const int limit = std::min(3, static_cast<int>(visible_corners.size()));
    const size_t next = rand() % limit;

    next_corner = static_cast<int>(visible_corners[next].index);
}

bool NPC::is_visible(const Corner& _origin, const Corner& _traslation) const {
    b2Vec2 origin = {_origin.x, _origin.y};
    b2Vec2 traslation = {_traslation.x, _traslation.y};
    b2QueryFilter filter = b2DefaultQueryFilter();

    b2RayResult result = b2World_CastRayClosest(world, origin, traslation, filter);

    return result.shapeId.index1 == 0;
}

CarDTO NPC::get_state() {
    return car.get_state();
}
