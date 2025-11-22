#include "car_npc.h"

#include "src/common/DTO.h"
#include "src/common/constants.h"

#include "parser_yaml.h"

CarNPC::CarNPC(Corner& start_corner, b2WorldId& world) {
    b2BodyDef body = b2DefaultBodyDef();
    body.type = b2_dynamicBody;
    body.linearDamping = 2.0f;
    body.angularDamping = 7.0f;
    body.position = {start_corner.x,start_corner.y};
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

void CarNPC::move(Corner& target) {
    rotate(target);
    b2Vec2 vel = b2Body_GetLinearVelocity(body_id);
    float speed = b2Length(vel);

    float accel = 80.0f;
    float limit = MAX_SPEED;

    if (speed < limit) {
        b2Vec2 velocity = b2Body_GetWorldVector(body_id, {1,0});
        b2Vec2 force = {velocity.x * accel, velocity.y * accel};
        b2Body_ApplyForceToCenter(body_id, force, true);
    }
}

void CarNPC::rotate(Corner& target) {
    b2Vec2 pos = b2Body_GetPosition(body_id);
    b2Vec2 dir = {target.x - pos.x, target.y - pos.y};

    float target_angle = atan2f(dir.x, dir.y);
    b2Rot rot = b2Body_GetRotation(body_id);
    float car_angle = atan2(rot.s, rot.c);

    float diff = target_angle - car_angle;

    b2Body_SetAngularVelocity(body_id, diff *4);
}

CarDTO CarNPC::get_state() {
    b2Vec2 pos = get_position();
    float x = pos.x;
    float y = pos.y;
    b2Rot rot = b2Body_GetRotation(body_id);
    float angle = atan2(rot.s, rot.c);

    return {x, y,0, angle, 0, false, 0, 0, 0, NPC, 0};
}
