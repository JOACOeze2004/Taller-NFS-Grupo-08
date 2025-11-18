#ifndef TALLER_TP_CAR_H
#define TALLER_TP_CAR_H

#include "car_state.h"
#include "../common/car_DTO.h"
#include "../common/constants.h"
#include <box2d/box2d.h>

constexpr float MASS = 20.f;
constexpr float HANDLING = 80.0f + MASS;
constexpr float ACCELERATION = 80.0f - MASS;
constexpr float BRAKING = 20.0f - MASS/2;

class Car {
    float NITRO = MAX_NITRO;
    float max_speed = MAX_SPEED;
    CarState state;
    float mass;
    float handling = HANDLING;
    float acceleration = ACCELERATION;
    float braking = BRAKING;
    int car_id;
    int life = MAX_LIFE;
    int nitro = NITRO;
    b2BodyId body_id;
    bool nitro_activated = false;
    int nitro_consuption = NITRO_CONSUMPTION;
    int remaining_upgrades;
    
    float base_mass;
    float base_handling;
    float base_acceleration;
    float base_braking;
    
    int acceleration_upgrades_applied = 0;
    int handling_upgrades_applied = 0;
    int nitro_upgrades_applied = 0;
    int life_upgrades_applied = 0;
    int brake_upgrades_applied = 0;
    int mass_upgrades_applied = 0;

public:
    explicit Car(b2WorldId world, float _mass, float _handling, float _acceleration, float _braking, int _car_id);
    ~Car();
    void accelerate();
    void brake();
    void turn_right();
    void turn_left();
    void apply_friction();
    void handle_hit(b2Vec2& normal, float& force, bool is_hitter);
    b2Vec2 get_position();
    bool is_dead();
    void delete_from_map();

    void restore_life();
    void activate_infinite_life();
    void activate_infinite_nitro();

    void activate_lose_race();
    void activate_win_race();

    void toggle_nitro_status();
    void apply_nitro_force();
    void update_nitro_usage();

    CarDTO get_state() const;
    void set_spawn(float& x, float& y, float& angle_x, float& angle_y);
    
    void reset_stats_and_upgrades();

    void accelerate_upgrade();
    void handling_upgrade();
    void nitro_upgrade();
    void life_upgrade();
    void brake_upgrade();
    void mass_upgrade();
    
    void accelerate_downgrade();
    void handling_downgrade();
    void nitro_downgrade();
    void life_downgrade();
    void brake_downgrade();
    void mass_downgrade();
    
    bool upgrade(float& stat, float upgrade_factor);
    bool downgrade(float& stat, float upgrade_factor, int& upgrades_applied);

private:
    float calculate_torque() const;
};


#endif  // TALLER_TP_CAR_H
