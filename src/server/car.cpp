#include "car.h"

#include <cmath>
#include <iostream>
#include <ostream>

Car::Car(b2WorldId world){
    b2BodyDef body = b2DefaultBodyDef();
    body.type = b2_dynamicBody;
    body.linearDamping = 2.0f;
    body.angularDamping = 5.0f;
    body.position = {300.0f,300.0f};
    body_id = b2CreateBody(world, &body);
    b2Body_EnableContactEvents(body_id, true);
    b2Body_EnableHitEvents(body_id, true);
    b2Polygon box = b2MakeBox(10.0f, 5.0f);
    b2ShapeDef shape_def = b2DefaultShapeDef();
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
    if (speed < max_speed) {
        b2Vec2 velocity = b2Body_GetWorldVector(body_id, {1,0});
        b2Vec2 force = {velocity.x * acceleration, velocity.y * acceleration};
        b2Body_ApplyForceToCenter(body_id, force, true);
    }
}

void Car::brake() {
    b2Vec2 vel = b2Body_GetWorldVector(body_id, {1,0});
    b2Vec2 brake = {-vel.x * braking, -vel.y * braking};
    b2Body_ApplyForceToCenter(body_id, brake, true);
}

void Car::turn_right() {
    b2Vec2 vel = b2Body_GetLinearVelocity(body_id);
    float speed = b2Length(vel);
    if (speed > 1.0f) {
        b2Body_ApplyTorque(body_id, handling, true);
    }
}

void Car::turn_left() {
    b2Vec2 vel = b2Body_GetLinearVelocity(body_id);
    float speed = b2Length(vel);
    if (speed > 1.0f) {
        b2Body_ApplyTorque(body_id, -handling, true);
    }
}

void Car::activate_win_race() {
    life = 26062011;
}

void Car::restore_life() {
    life = MAX_LIFE/2;
}

void Car::activate_infinite_life() {
    life = MAX_LIFE; //hacer un valor muy grande o alguno que sepamos representa la vida infinita
}

void Car::activate_lose_race() {
    life = 0;
}

void Car::activate_infinite_nitro() {
    nitro = MAX_NITRO; //hacer un valor muy grande o alguno que sepamos representa el nitro infinito
}

void Car::update_position() {

}

CarDTO Car::get_state() const {
    b2Vec2 pos = b2Body_GetPosition(body_id);
    b2Rot rot = b2Body_GetRotation(body_id);
    float angle = atan2(rot.s, rot.c);
    b2Vec2 vel = b2Body_GetLinearVelocity(body_id);
    float speed = b2Length(vel);
    return CarDTO(pos.x, pos.y, speed, angle);
}