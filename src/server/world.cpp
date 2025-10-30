#include "world.h"

#include <iostream>
#include <ostream>

World::World() {
    b2WorldDef world_def = b2DefaultWorldDef();
    world_def.gravity = {0.0f, 0.0f};
    world_id = b2CreateWorld(&world_def);
}

void World::update() {
    float dt = 1.0f / 60.0f;
    const int sub_step_count = 4;
    b2World_Step(world_id, dt, sub_step_count);
}

b2WorldId World::get_id() {
    return world_id;
}

World::~World() {
    b2DestroyWorld(world_id);
}