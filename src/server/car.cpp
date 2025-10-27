#include "car.h"

#include <cmath>

void Car::accelerate() {
    state.velocity += acceleration;

    if (state.velocity > max_speed) {
        state.velocity = max_speed;
    }
}

void Car::brake() {
    state.velocity -= braking;

    if (state.velocity < -max_speed) {
        state.velocity = -max_speed;
    }
}

void Car::turn_right() {
    state.angle += handling;
}

void Car::turn_left() {
    state.angle -= handling;
}

void Car::update_position() {
    state.x += state.velocity * cos(state.angle);
    state.y += state.velocity * sin(state.angle);

    state.velocity = state.velocity * 0.95;
    if (std::abs(state.velocity) < 0.1) {
        state.velocity = 0;
    }
}

CarDTO Car::get_state() const {
    return CarDTO(state.x, state.y, state.velocity, state.angle);
}