#ifndef TALLER_TP_CAR_H
#define TALLER_TP_CAR_H

#include "car_state.h"
#include "../common/car_DTO.h"
#include "../common/constants.h"

constexpr float HANDLING = 0.1f;
constexpr float ACCELERATION = 0.5f;
constexpr float BRAKING = 0.3f;

class car {
    float max_speed = MAX_SPEED;
    CarState state;
    float handling = HANDLING;
    float acceleration = ACCELERATION;
    float braking = BRAKING;

public:
    void accelerate();
    void brake();
    void turn_right();
    void turn_left();
    void update_position();

    CarDTO get_state() const;

    float get_velocity();
    float get_angle();
};


#endif  // TALLER_TP_CAR_H
