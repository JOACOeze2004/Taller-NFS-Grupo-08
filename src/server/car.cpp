#include "car.h"

#include <cmath>
#include <iostream>

#include "src/common/DTO.h"

Car::Car(b2WorldId world, float _mass, float _handling, float _acceleration, float _braking, int _car_id) : mass(_mass), handling(_handling + mass), acceleration(_acceleration - mass), braking(_braking - mass/2), car_id(_car_id){

    base_mass = _mass;
    base_handling = _handling;
    base_acceleration = _acceleration;
    base_braking = _braking;

    handling = base_handling + mass;
    acceleration = base_acceleration - mass;
    braking = base_braking - mass/2;
    
    b2BodyDef body = b2DefaultBodyDef();
    body.type = b2_dynamicBody;
    body.linearDamping = 2.0f;
    body.angularDamping = 7.0f;
    body.position = {Config::instance().car.spawn_position[0],Config::instance().car.spawn_position[1]};
    body_id = b2CreateBody(world, &body);
    b2Body_EnableContactEvents(body_id, true);
    b2Body_EnableHitEvents(body_id, true);

    b2Body_SetUserData(body_id, this);

    b2Polygon box = b2MakeBox(10.0f, 5.0f);
    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.density = 0.0006f;
    b2ShapeId shape = b2CreatePolygonShape(body_id, &shape_def, &box);
    b2Shape_EnableContactEvents(shape, true);
    b2Shape_EnableHitEvents(shape, true);

    remaining_upgrades = 3;

    initialize_upgrade_actions();
}

Car::~Car() {
    if (b2Body_IsValid(body_id)) {
        b2DestroyBody(body_id);
    }
}

void Car::reset_upgrades() {
    upgrades[ACCELERATION_UPGRADE] = acceleration_upgrades_applied;
    upgrades[MASS_UPGRADE] = mass_upgrades_applied;
    upgrades[NITRO_UPGRADE] = nitro_upgrades_applied;
    upgrades[LIFE_UPGRADE] = life_upgrades_applied;
    upgrades[BRAKE_UPGRADE] = brake_upgrades_applied;
    upgrades[HANDLING_UPGRADE] = handling_upgrades_applied;
    upgrades[NONE_UPGRADE] = 0;
}

void Car::initialize_upgrade_actions() {
    upgrade_actions[ACCELERATION_UPGRADE] = {
        [this]() { accelerate_upgrade(); },
        [this]() { accelerate_downgrade(); }
    };
    upgrade_actions[HANDLING_UPGRADE] = {
        [this]() { handling_upgrade(); },
        [this]() { handling_downgrade(); }
    };
    upgrade_actions[NITRO_UPGRADE] = {
        [this]() { nitro_upgrade(); },
        [this]() { nitro_downgrade(); }
    };
    upgrade_actions[LIFE_UPGRADE] = {
        [this]() { life_upgrade(); },
        [this]() { life_downgrade(); }
    };
    upgrade_actions[BRAKE_UPGRADE] = {
        [this]() { brake_upgrade(); },
        [this]() { brake_downgrade(); }
    };
    upgrade_actions[MASS_UPGRADE] = {
        [this]() { mass_upgrade(); },
        [this]() { mass_downgrade(); }
    };
}

void Car::accelerate() {
    b2Vec2 vel = b2Body_GetLinearVelocity(body_id);
    float speed = b2Length(vel);

    float accel = acceleration;
    float limit = max_speed;
    
    if (speed < limit) {
        b2Vec2 velocity = b2Body_GetWorldVector(body_id, {1,0});
        b2Vec2 force = {velocity.x * accel, velocity.y * accel};
        b2Body_ApplyForceToCenter(body_id, force, true);
    }
    apply_friction();
}

void Car::brake() {
    b2Vec2 vel = b2Body_GetLinearVelocity(body_id);
    b2Vec2 vec = b2Body_GetWorldVector(body_id, {1, 0});
    float speed = b2Dot(vel, vec);
    b2Vec2 brake;

    if (speed < 0) {
        brake = {-vec.x * braking * 2, -vec.y * braking * 2};
    }else{
        brake = {-vec.x * braking, -vec.y * braking};
    }

    b2Body_ApplyForceToCenter(body_id, brake, true);
    apply_friction();
}

void Car::turn_right() {
    float torque  = calculate_torque();

    b2Body_ApplyTorque(body_id, torque, true);
    apply_friction();
}

void Car::turn_left() {
    float torque  = calculate_torque();

    b2Body_ApplyTorque(body_id, -torque, true);
    apply_friction();
}

float Car::calculate_torque() const {
    b2Vec2 vel = b2Body_GetLinearVelocity(body_id);
    float speed = b2Length(vel);
    float torque  = 0;

    if (speed > 50.0f) {
        torque = handling;
    }
    else if (speed > 25.0f) {
        torque = handling * 0.7f;
    }
    else if (speed > 10.0f) {
        torque = handling * 0.4f;
    }

    return torque;
}

void Car::restore_life() { life = max_life; }

void Car::activate_infinite_life() {
    if (life >= max_life + 1)
        life = max_life;
    else {
        life = max_life + 1;
    }
}

void Car::activate_lose_race() { life = 0; }

void Car::activate_infinite_nitro() {
    if (nitro_recharge > Config::instance().car.nitro_recharge_rate) {
        nitro_recharge = Config::instance().car.nitro_recharge_rate;
    } else {
        nitro_recharge = Config::instance().car.nitro_recharge_rate * 50;
    }
}

void Car::apply_friction() {
    b2Vec2 right = b2Body_GetWorldVector(body_id, {0,1});

    b2Vec2 vel = b2Body_GetLinearVelocity(body_id);

    float lateral_speed = b2Dot(right, vel);

    b2Vec2 lateral_impulse = {
        -lateral_speed*right.x*b2Body_GetMass(body_id),
        -lateral_speed*right.y*b2Body_GetMass(body_id),
    };
    b2Body_ApplyLinearImpulseToCenter(body_id, lateral_impulse, true);
}

void Car::handle_hit(b2Vec2& normal, float& force, bool is_hitter) {
    if (in_lobby){
        return;
    }
    
    b2Vec2 impulse = {normal.x * force / MASS, normal.y * force / MASS};
    b2Body_ApplyForceToCenter(body_id, impulse, true);

    if (life > max_life) {
        return;
    }

    b2Vec2 forward = b2Body_GetWorldVector(body_id, {1.0f, 0.0f});
    float angle_impact = fabs(b2Dot(normal, forward));

    float angle_factor = 0.3f + 0.7f * angle_impact;

    if (is_hitter) {
        life -= static_cast<int>((force / (MASS*Config::instance().car.hitter_factor)) * angle_factor);
    }
    else {
        life -= static_cast<int>((force / (MASS * Config::instance().car.hitted_factor)) * angle_factor);
    }

    if (life < 0) {
        life = 0;
    }
}

void Car::toggle_nitro_status(){
    if (nitro < Config::instance().car.max_nitro && !nitro_activated){
        return;
    }
    nitro_activated = true;
}

void Car::apply_nitro_force() {
    if (!nitro_activated){
        return;
    }

    b2Vec2 vel = b2Body_GetLinearVelocity(body_id);
    float speed = b2Length(vel);
    float limit = max_speed * 3.0f;

    float speed_factor = 1.0f; 
    if (speed > 50.0f) {
        speed_factor = 0.9f;
    } else if (speed > 25.0f) {
        speed_factor = 1.4f;
    } else if (speed > 10.0f) {
        speed_factor = 1.8f;
    }

    if (speed < 1.0f) {
        b2Vec2 forward = b2Body_GetWorldVector(body_id, {1,0});
        b2Vec2 boost = {forward.x * acceleration * Config::instance().car.boost_factor, forward.y * acceleration * Config::instance().car.boost_factor};
        b2Body_ApplyLinearImpulseToCenter(body_id, boost, true);
        return;
    }

    if (speed < limit) {
        b2Vec2 forward = b2Body_GetWorldVector(body_id, {1,0});
        b2Vec2 force = {forward.x * acceleration * speed_factor, forward.y * acceleration * speed_factor};
        b2Body_ApplyForceToCenter(body_id, force, true);
    }
}

void Car::update_nitro_usage(){
    if (nitro_activated){
        nitro -= nitro_consuption;
        if (nitro <= 0){
            nitro = 0;
            nitro_activated = false;
        }
    }else{
        nitro += nitro_recharge;        
        if (nitro >= max_nitro){
            nitro = max_nitro;
        }        
    }
}

void Car::recalculate_stats() {
    float handling_base = base_handling + mass;
    float acceleration_base = base_acceleration - mass;
    float braking_base = base_braking - mass / 2.0f;

    float handling_mult = 1.0f;
    for (int i = 0; i < handling_upgrades_applied; ++i) {
        handling_mult *= Config::instance().car.handling_upgrade_factor;
    }

    float acceleration_mult = 1.0f;
    for (int i = 0; i < acceleration_upgrades_applied; ++i) {
        acceleration_mult *= Config::instance().car.acceleration_upgrade_factor;
    }

    float braking_mult = 1.0f;
    for (int i = 0; i < brake_upgrades_applied; ++i) {
        braking_mult *= Config::instance().car.brake_upgrade_factor;
    }

    handling = handling_base * handling_mult;
    acceleration = acceleration_base * acceleration_mult;
    braking = braking_base * braking_mult;
}

CarDTO Car::get_state() {
    b2Vec2 pos = b2Body_GetPosition(body_id);
    b2Rot rot = b2Body_GetRotation(body_id);
    float angle = atan2(rot.s, rot.c);
    b2Vec2 vel = b2Body_GetLinearVelocity(body_id);
    float speed = b2Length(vel);
    float life_percentage = (static_cast<float>(life) * 100.0f) / static_cast<float>(Config::instance().car.max_life);
    return CarDTO(pos.x, pos.y, speed, angle, car_id, false, life_percentage, this->nitro_activated, this->nitro, IN_GAME, remaining_upgrades);
}

std::map<Upgrades, int> Car::get_upgrades() {
    reset_upgrades();
    return upgrades;
}

b2Vec2 Car::get_position() const { return b2Body_GetPosition(body_id); }

bool Car::is_dead() { return life == 0; }

void Car::delete_from_map() {
    if (b2Body_IsValid(body_id)) {
        b2Body_Disable(body_id);
    }
}

void Car::set_spawn(float& x, float& y, float& angle_x, float& angle_y) {
    in_lobby = false;
    float dx = x - angle_x;
    float dy = -(y - angle_y);
    float angle = atan2f(dx, dy);
    b2Body_SetLinearVelocity(body_id, {0,0});
    b2Body_SetAngularVelocity(body_id, 0);
    b2Rot rot = b2MakeRot(angle + std::numbers::pi/2);
    life = max_life;
    nitro = max_nitro;
    nitro_activated = false;
    b2Body_SetTransform(body_id, {x,y}, rot);
}

void Car::reset_stats_and_upgrades() {
    mass = base_mass;
    handling = base_handling + mass;
    acceleration = base_acceleration - mass;;
    braking = base_braking - mass/2;;
    
    max_life = Config::instance().car.max_life;
    life = Config::instance().car.max_life;
    max_nitro = Config::instance().car.max_nitro;
    nitro = Config::instance().car.max_nitro;
    nitro_activated = false;
    
    remaining_upgrades = 3;
    acceleration_upgrades_applied = 0;
    handling_upgrades_applied = 0;
    nitro_upgrades_applied = 0;
    life_upgrades_applied = 0;
    brake_upgrades_applied = 0;
    mass_upgrades_applied = 0;
}

void Car::accelerate_upgrade() {
    if (upgrade(acceleration, Config::instance().car.acceleration_upgrade_factor)) {
        acceleration_upgrades_applied++;
    }
}

void Car::handling_upgrade() {
    if (upgrade(handling, Config::instance().car.handling_upgrade_factor)) {
        handling_upgrades_applied++;
    }
}

void Car::nitro_upgrade() {
    if (upgrade(max_nitro, Config::instance().car.nitro_upgrade_factor)) {
        nitro_upgrades_applied++;
        nitro = max_nitro;
    }
}

void Car::life_upgrade() {
    float max_life_casted = static_cast<float>(max_life);
    if (upgrade(max_life_casted, Config::instance().car.life_upgrade_factor)) {
        max_life = static_cast<int>(max_life_casted);
        life = max_life;
        life_upgrades_applied++;
    }
}

void Car::brake_upgrade() {
    if (upgrade(braking, Config::instance().car.brake_upgrade_factor)) {
        brake_upgrades_applied++;
    }
}

void Car::mass_upgrade() {
    if (upgrade(mass, Config::instance().car.mass_upgrade_factor)) {
        mass_upgrades_applied++;
        recalculate_stats();
    }
}

bool Car::upgrade(float& stat, float upgrade_factor) {
    if (remaining_upgrades <= 0) {
        return false;
    }
    stat *= upgrade_factor;
    remaining_upgrades -= 1;
    return true;
}

void Car::accelerate_downgrade() {
    downgrade(acceleration, Config::instance().car.acceleration_upgrade_factor, acceleration_upgrades_applied);
}

void Car::handling_downgrade() {
    downgrade(handling, Config::instance().car.handling_upgrade_factor, handling_upgrades_applied);
}

void Car::nitro_downgrade() {
    float max_nitro_casted = static_cast<float>(max_nitro);
    if (downgrade(max_nitro_casted, Config::instance().car.nitro_upgrade_factor, nitro_upgrades_applied)) {
        max_nitro = static_cast<int>(max_nitro_casted);
        if (nitro > max_nitro) {
            nitro = max_nitro;
        }
    }
}

void Car::life_downgrade() {
    float max_life_casted = static_cast<float>(max_life);
    if (downgrade(max_life_casted, Config::instance().car.life_upgrade_factor, life_upgrades_applied)) {
        max_life = static_cast<int>(max_life_casted);
        if (life > max_life) {
            life = max_life;
        }
    }
}

void Car::brake_downgrade() {
    downgrade(braking, Config::instance().car.brake_upgrade_factor, brake_upgrades_applied);
}

void Car::mass_downgrade() {
    if (downgrade(mass, Config::instance().car.mass_upgrade_factor, mass_upgrades_applied)){
        recalculate_stats();
    }
}

bool Car::downgrade(float& stat, float upgrade_factor, int& upgrades_applied) {
    if (upgrades_applied <= 0) {
        return false;
    }
    
    stat /= upgrade_factor;
    
    remaining_upgrades += 1;
    upgrades_applied -= 1;
    return true;
}

bool Car::apply_upgrade(Upgrades type, bool is_upgrade) {
    auto it = upgrade_actions.find(type);
    if (it != upgrade_actions.end()) {
        int upgrades_before = remaining_upgrades;
        
        (is_upgrade ? it->second.first : it->second.second)();
        
        int upgrades_after = remaining_upgrades;        
        if (is_upgrade) {
            return upgrades_after < upgrades_before; 
        } else {
            return upgrades_after > upgrades_before;
        }
    }
    return false;
}

int Car::get_id() const { return this->car_id; }

void Car::set_position(float x, float y, float look_x, float look_y) {
    float dx = look_x - x;
    float dy = look_y - y;
    float angle = atan2f(-dx, dy);  

    b2Body_SetLinearVelocity(body_id, {0,0});
    b2Body_SetAngularVelocity(body_id, 0);

    b2Rot rot = b2MakeRot(angle + std::numbers::pi/2);

    b2Body_SetTransform(body_id, {x,y}, rot);
}