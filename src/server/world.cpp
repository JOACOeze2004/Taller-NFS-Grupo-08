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
        map = YAML::LoadFile("../src/server/sanandreas.yaml");
    }
    if (map_name == "Vice City") {
        map = YAML::LoadFile("../src/server/vicecity.yaml");
    }

    std::vector<StaticBody> boxes;
    for (const auto& layer : map["layers"]) {
        if (layer["type"].as<std::string>() != "objectgroup") continue;
        for (const auto& obj : layer["objects"]) {
            StaticBody box;
            box.id = obj["id"].as<int>();
            box.x = obj["x"].as<float>();
            box.y = obj["y"].as<float>();
            box.width = obj["width"].as<float>();
            box.height = obj["height"].as<float>();
            boxes.push_back(box);
        }
    }

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
    }
}

World::~World() {
    b2DestroyWorld(world);
}