#include "car.h"

#include <cmath>
#include <iostream>
#include <ostream>

Car::Car(b2WorldId world){
    b2BodyDef body = b2DefaultBodyDef();
    body.type = b2_dynamicBody;
    body.linearDamping = 2.0f;
    body.angularDamping = 5.0f;
    body.position = {0.0f,0.0f};
    body_id = b2CreateBody(world, &body);

    b2Polygon box = b2MakeBox(1.0f, 2.0f);
    b2ShapeDef shape_def = b2DefaultShapeDef();
    b2CreatePolygonShape(body_id, &shape_def, &box);
}

Car::~Car() {
    b2DestroyBody(body_id);
}

void Car::accelerate() {
    b2Vec2 forward = b2Body_GetWorldVector(body_id, {0,1});
    b2Vec2 force = {forward.x * acceleration, forward.y * acceleration};
    b2Body_ApplyLinearImpulse(body_id, force, b2Body_GetPosition(body_id), true);
}

void Car::brake() {
    b2Vec2 vel = b2Body_GetWorldVector(body_id, {0,1});
    b2Vec2 brake = {-vel.x * braking, -vel.y * braking};
    b2Body_ApplyLinearImpulse(body_id, brake, b2Body_GetPosition(body_id), true);
}

void Car::turn_right() {
    b2Rot rot = b2Body_GetRotation(body_id);
    float angle = atan2(rot.s, rot.c);
    float new_angle = angle + handling;
    b2Rot new_rot = b2MakeRot(new_angle);
    b2Body_SetTransform(body_id, b2Body_GetPosition(body_id), new_rot);
}

void Car::turn_left() {
    b2Rot rot = b2Body_GetRotation(body_id);
    float angle = atan2(rot.s, rot.c);
    float new_angle = angle - handling;
    b2Rot new_rot = b2MakeRot(new_angle);
    b2Body_SetTransform(body_id, b2Body_GetPosition(body_id), new_rot);
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
    float speed = sqrtf(vel.x*vel.x + vel.y*vel.y);
    return CarDTO(pos.x, pos.y, speed, angle);
}