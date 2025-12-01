#ifndef TALLER_TP_CAR_H
#define TALLER_TP_CAR_H

#include <box2d/box2d.h>

#include "../common/DTO.h"
#include "../common/car_DTO.h"

#include "car_state.h"
#include "config.h"
#include "functional"

constexpr float MASS = 20.f;
constexpr float HANDLING = 80.0f + MASS;
constexpr float ACCELERATION = 80.0f - MASS;
constexpr float BRAKING = 20.0f - MASS/2;

class Car {
    float max_nitro = Config::instance().car.max_nitro;
    float max_speed = Config::instance().car.max_speed;
    float max_life = Config::instance().car.max_life;

    CarState state;
    float mass;
    float handling = HANDLING;
    float acceleration = ACCELERATION;
    float braking = BRAKING;
    int car_id;
    int life = max_life;
    int nitro = Config::instance().car.max_nitro;
    b2BodyId body_id;
    bool nitro_activated = false;
    int nitro_consuption = Config::instance().car.nitro_consumption;
    int nitro_recharge = Config::instance().car.nitro_recharge_rate;
    int remaining_upgrades;
    bool in_lobby = true;
    
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

    std::map<Upgrades, int> upgrades;

    void reset_upgrades();
    void initialize_upgrade_actions();
    using UpgradeAction = std::function<void()>;
    std::unordered_map<Upgrades, std::pair<UpgradeAction, UpgradeAction>> upgrade_actions;
    void recalculate_stats();

public:
    explicit Car(b2WorldId world, float _mass, float _handling, float _acceleration, float _braking, int _car_id);
    ~Car();
    void accelerate();
    void brake();
    void turn_right();
    void turn_left();
    void apply_friction();
    void handle_hit(b2Vec2& normal, float& force, bool is_hitter);
    b2Vec2 get_position() const;
    bool is_dead();
    void delete_from_map();

    void restore_life();
    void activate_infinite_life();
    void activate_infinite_nitro();

    void activate_lose_race();

    void toggle_nitro_status();
    void apply_nitro_force();
    void update_nitro_usage();

    CarDTO get_state();
    std::map<Upgrades, int> get_upgrades();
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

    bool apply_upgrade(Upgrades type, bool is_upgrade);
    int get_id() const;
    void set_position(float x, float y, float look_x, float look_y);


private:
    float calculate_torque() const;
};


#endif  // TALLER_TP_CAR_H
