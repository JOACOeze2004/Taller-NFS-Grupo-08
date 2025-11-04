#include "world.h"
#include <vector>

World::World() {
    b2WorldDef world_def = b2DefaultWorldDef();
    world_def.gravity = {0.0f, 0.0f};
    world = b2CreateWorld(&world_def);
}

void World::update() {
    float dt = 1.0f / 60.0f;
    const int sub_step_count = 4;
    b2World_Step(world, dt, sub_step_count);
}

b2WorldId World::get_id() {
    return world;
}

void World::generate_map(std::string& map_name) {
    std::vector<StaticBody> boxes = parser.parse_map(map_name);

    create_collisions(boxes);
}

void World::create_collisions(std::vector<StaticBody>& boxes) {
    for (const auto& box : boxes) {

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_staticBody;

        bodyDef.position = { (box.x + box.width/2), (box.y + box.height/2) };

        b2BodyId body = b2CreateBody(world, &bodyDef);
        b2Body_EnableContactEvents(body, true);
        b2Body_EnableHitEvents(body, true);

        b2Polygon b2box = b2MakeBox(box.width/2, box.height/2);
        b2ShapeDef shape_def = b2DefaultShapeDef();
        b2ShapeId shape = b2CreatePolygonShape(body, &shape_def, &b2box);
        b2Shape_EnableContactEvents(shape, true);
        b2Shape_EnableHitEvents(shape, true);

        bodies.push_back(body);
    }
}

World::~World() {
    for (auto& body : bodies) {
        b2DestroyBody(body);
    }
    b2DestroyWorld(world);
}