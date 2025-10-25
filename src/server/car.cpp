#include "car.h"

#include <cmath>

void car::accelerate() {
    state.velocity += acceleration;

    if (state.velocity > max_speed) {
        state.velocity = max_speed;
    }
}

void car::brake() {
    state.velocity -= braking;

    if (state.velocity < -max_speed) {
        state.velocity = -max_speed;
    }
}

void car::turn_right() {
    state.angle += handling;
}

void car::turn_left() {
    state.angle -= handling;
}

void car::update_position() {
    state.x += state.velocity * cos(state.angle);
    state.y += state.velocity * sin(state.angle);

    state.velocity = state.velocity * 0.95;
    if (std::abs(state.velocity) < 0.1) {
        state.velocity = 0;
    }
}

CarDTO car::get_state() const {
    return CarDTO(state.x, state.y, state.velocity, state.angle);
}

float car::get_velocity() {
    return state.velocity;
}

float car::get_angle() {
    return state.angle;
}