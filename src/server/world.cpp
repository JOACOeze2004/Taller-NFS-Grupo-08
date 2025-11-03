#include "world.h"

#include <iostream>
#include <ostream>
#include <vector>
#include <yaml-cpp/yaml.h>

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

struct StaticBody {
    int id;
    float x;
    float y;
    float width;
    float height;
};

void World::generate_map(std::string map_name) {
    YAML::Node map;
    if (map_name == "Liberty City") {
        map = YAML::LoadFile("../src/server/libertycity.yaml");
    }
    if (map_name == "San Andreas") {
        map = YAML::LoadFile("libertycity.yaml");
    }
    if (map_name == "Vice City") {
        map = YAML::LoadFile("libertycity.yaml");
    }

    std::vector<StaticBody> edificios;
    for (const auto& layer : map["layers"]) {
        if (layer["type"].as<std::string>() != "objectgroup") continue;
        for (const auto& obj : layer["objects"]) {
            StaticBody e;
            e.id = obj["id"].as<int>();
            e.x = obj["x"].as<float>();
            e.y = obj["y"].as<float>();
            e.width = obj["width"].as<float>();
            e.height = obj["height"].as<float>();
            edificios.push_back(e);
        }
    }

    for (const auto& e : edificios) {

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_staticBody;

        bodyDef.position = { (e.x + e.width/2), (e.y + e.height/2) };

        b2BodyId body = b2CreateBody(world, &bodyDef);
        b2Body_EnableContactEvents(body, true);
        b2Body_EnableHitEvents(body, true);

        b2Polygon box = b2MakeBox(e.width/2, e.height/2);
        b2ShapeDef shape_def = b2DefaultShapeDef();
        b2ShapeId shape = b2CreatePolygonShape(body, &shape_def, &box);
        b2Shape_EnableContactEvents(shape, true);
        b2Shape_EnableHitEvents(shape, true);
    }
}

World::~World() {
    b2DestroyWorld(world);
}