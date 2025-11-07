#ifndef TALLER_TP_CAR_H
#define TALLER_TP_CAR_H

#include "car_state.h"
#include "../common/car_DTO.h"
#include "../common/constants.h"
#include <box2d/box2d.h>

constexpr float HANDLING = 180.0f;
constexpr float ACCELERATION = 80.0f;
constexpr float BRAKING = 20.0f;

class Car {
    float max_speed = MAX_SPEED;
    CarState state;
    float handling = HANDLING;
    float acceleration = ACCELERATION;
    float braking = BRAKING;
    int life = MAX_LIFE;
    int nitro = MAX_NITRO;
    b2BodyId body_id;
    bool nitro_activated = false;
    int nitro_consuption = NITRO_CONSUMPTION;

public:
    explicit Car(b2WorldId world);
    ~Car();
    void accelerate();
    void brake();
    void turn_right();
    void turn_left();
    void update_position();
    void handle_hit(b2Vec2& normal, float& force, bool is_hitter);

    void restore_life();
    void activate_infinite_life();
    void activate_infinite_nitro();

    //ver como manejar estos casos seguro se manejan desde el gameloop
    void activate_lose_race();
    void activate_win_race();

    void toggle_nitro_status();
    void update_nitro_usage();

    int get_nitro() const;

    

    CarDTO get_state() const;
};


#endif  // TALLER_TP_CAR_H
