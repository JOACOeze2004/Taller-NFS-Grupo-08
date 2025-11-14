#include "car.h"

#include <cmath>
#include <iostream>

Car::Car(b2WorldId world, float _mass, float _handling, float _acceleration, float _braking, int _car_id) : mass(_mass), handling(_handling + mass), acceleration(_acceleration - mass), braking(_braking - mass/2), car_id(_car_id){
    body = b2DefaultBodyDef();
    body.type = b2_dynamicBody;
    body.linearDamping = 2.0f;
    body.angularDamping = 7.0f;
    body.position = {300.0f,300.0f};
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
}

Car::~Car() {
    if (b2Body_IsValid(body_id)) {
        b2DestroyBody(body_id);
    }
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

void Car::activate_win_race() {
    life = 26062011;
}

void Car::restore_life() {
    life = MAX_LIFE/2;
}

void Car::activate_infinite_life() {
    if (life >= MAX_LIFE + 1)
        life = MAX_LIFE;
    else {
        life = MAX_LIFE + 1;
    }
}

void Car::activate_lose_race() {
    life = 0;
}

void Car::activate_infinite_nitro() {
    if (nitro >= MAX_NITRO+1) {
        nitro = MAX_NITRO;
    }
    else {
        nitro = MAX_NITRO + 1;
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
    b2Vec2 impulse = {normal.x * force / MASS, normal.y * force / MASS};
    b2Body_ApplyForceToCenter(body_id, impulse, true);

    if (life > MAX_LIFE) {
        return;
    }

    b2Vec2 forward = b2Body_GetWorldVector(body_id, {1.0f, 0.0f});
    float angle_impact = fabs(b2Dot(normal, forward));
    float angle_factor = 0.3f + 0.7f * angle_impact;

    if (is_hitter) {
        life -= static_cast<int>((force / (MASS*0.2)) * angle_factor);
    }
    else {
        life -= static_cast<int>((force / (MASS * 0.1f)) * angle_factor);
    }

    if (life < 0) {
        life = 0; // matarlo
    }
}

void Car::toggle_nitro_status(){
    if (nitro < MAX_NITRO && !nitro_activated){
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
        b2Vec2 boost = {forward.x * acceleration * 2.5f, forward.y * acceleration * 2.5f};
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
    if (nitro > MAX_NITRO) {
        return;
    }

    if (nitro_activated){
        nitro -= nitro_consuption;
        if (nitro <= 0){
            nitro = 0;
            nitro_activated = false;
        }
    }else{
        nitro += NITRO_RECHARGE_RATE;        
        if (nitro >= MAX_NITRO){
            nitro = MAX_NITRO;
        }        
    }
}

CarDTO Car::get_state() const {
    b2Vec2 pos = b2Body_GetPosition(body_id);
    b2Rot rot = b2Body_GetRotation(body_id);
    float angle = atan2(rot.s, rot.c);
    b2Vec2 vel = b2Body_GetLinearVelocity(body_id);
    float speed = b2Length(vel);
    return CarDTO(pos.x, pos.y, speed, angle, car_id, false, life, this->nitro_activated, this->nitro, IN_GAME);  //cambiar el car id y el under bridge para que funcione
}

b2Vec2 Car::get_position() {
    return b2Body_GetPosition(body_id);
}

bool Car::is_dead() {
    return life == 0;
}

void Car::delete_from_map() {
    if (b2Body_IsValid(body_id)) {
        b2Body_Disable(body_id);
    }
}

void Car::set_spawn(float& x, float& y, float& angle) {
    b2Body_SetLinearVelocity(body_id, {0,0});
    b2Body_SetAngularVelocity(body_id, 0);
    b2Rot rot = b2MakeRot(angle*(static_cast<float>(std::numbers::pi)));
    b2Body_SetTransform(body_id, {x,y}, rot);
}
