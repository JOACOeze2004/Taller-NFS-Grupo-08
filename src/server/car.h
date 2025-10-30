#ifndef TALLER_TP_CAR_H
#define TALLER_TP_CAR_H

#include "car_state.h"
#include "../common/car_DTO.h"
#include "../common/constants.h"
#include <box2d/box2d.h>
#include "world.h"

constexpr float HANDLING = 0.1f;
constexpr float ACCELERATION = 10000.0f;
constexpr float BRAKING = 5000.0f;

class Car {
    float max_speed = MAX_SPEED;
    CarState state;
    float handling = HANDLING;
    float acceleration = ACCELERATION;
    float braking = BRAKING;
    b2BodyId body_id;

public:
    explicit Car(b2WorldId world);
    ~Car();
    void accelerate();
    void brake();
    void turn_right();
    void turn_left();
    void update_position();

    CarDTO get_state() const;
};


#endif  // TALLER_TP_CAR_H
