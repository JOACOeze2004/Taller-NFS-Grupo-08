#include "car_npc.h"

#include "src/common/DTO.h"
#include "src/common/constants.h"

#include "config.h"
#include "parser_yaml.h"

CarNPC::CarNPC(GraphNode& start_corner, b2WorldId& world) {
    b2BodyDef body = b2DefaultBodyDef();
    body.type = b2_dynamicBody;
    body.linearDamping = 2.0f;
    body.angularDamping = 7.5f;
    body.position = {start_corner.x,start_corner.y};
    act_pos = {start_corner.x, start_corner.y};
    body_id = b2CreateBody(world, &body);
    b2Body_EnableContactEvents(body_id, true);
    b2Body_EnableHitEvents(body_id, true);

    b2Polygon box = b2MakeBox(10.0f, 5.0f);
    b2ShapeDef shape_def = b2DefaultShapeDef();
    shape_def.density = 0.0006f;
    b2ShapeId shape = b2CreatePolygonShape(body_id, &shape_def, &box);
    b2Shape_EnableContactEvents(shape, true);
    b2Shape_EnableHitEvents(shape, true);
}

b2Vec2 CarNPC::get_position() {
    return b2Body_GetPosition(body_id);
}

void CarNPC::move(GraphNode& target) {
    rotate(target);
    b2Vec2 vel = b2Body_GetLinearVelocity(body_id);
    float speed = b2Length(vel);

    float accel = 6.0f;
    float limit = Config::instance().car.max_speed;

    if (speed < limit) {
        b2Vec2 velocity = b2Body_GetWorldVector(body_id, {1,0});
        b2Vec2 force = {velocity.x * accel, velocity.y * accel};
        b2Body_ApplyForceToCenter(body_id, force, true);
    }
    rotate(target);
    apply_friction();
    last_pos = act_pos;
    act_pos = get_position();
}

void CarNPC::reverse() {
    float accel = -12.0f;
    b2Vec2 velocity = b2Body_GetWorldVector(body_id, {1,0});
    b2Vec2 force = {velocity.x * accel, velocity.y * accel};
    b2Body_ApplyForceToCenter(body_id, force, true);
}

float CarNPC::get_speed() {
    b2Vec2 vel = b2Body_GetLinearVelocity(body_id);
    float speed = b2Length(vel);
    return speed;
}

b2Vec2 CarNPC::get_last_pos() {
    return last_pos;
}

void CarNPC::rotate(GraphNode& target) {
    b2Vec2 pos = b2Body_GetPosition(body_id);
    b2Vec2 dir = { target.x - pos.x, target.y - pos.y };

    float targetAngle = atan2f(dir.y, dir.x);
    b2Rot rot = b2Body_GetRotation(body_id);

    float carAngle = atan2f(rot.s, rot.c);

    float diff = targetAngle - carAngle;

    while (diff >  std::numbers::pi) diff -= 2.0f * std::numbers::pi;
    while (diff < -std::numbers::pi) diff += 2.0f * std::numbers::pi;

    const float maxTorque = 50.0f;

    float torque = diff * 20.0f;

    if (torque >  maxTorque) torque =  maxTorque;
    if (torque < -maxTorque) torque = -maxTorque;

    b2Body_ApplyTorque(body_id, torque, true);
}

CarDTO CarNPC::get_state() {
    b2Vec2 pos = get_position();
    float x = pos.x;
    float y = pos.y;
    b2Rot rot = b2Body_GetRotation(body_id);
    float angle = atan2(rot.s, rot.c);
    return {x, y,0, angle, 0, false, 0, 0, 0, NPC_STATE, 0};
}

b2Vec2 CarNPC::get_forward() {
    return b2Body_GetWorldVector(body_id, {1, 0});
}

void CarNPC::apply_friction() {
    b2Vec2 right = b2Body_GetWorldVector(body_id, {0,1});

    b2Vec2 vel = b2Body_GetLinearVelocity(body_id);

    float lateral_speed = b2Dot(right, vel);

    b2Vec2 lateral_impulse = {
        -lateral_speed*right.x*b2Body_GetMass(body_id),
        -lateral_speed*right.y*b2Body_GetMass(body_id),
    };
    b2Body_ApplyLinearImpulseToCenter(body_id, lateral_impulse, true);
}

