#include "world.h"

#include <iostream>
#include <vector>

#include "car.h"


World::World() {
    b2WorldDef world_def = b2DefaultWorldDef();
    world_def.gravity = {0.0f, 0.0f};
    world = b2CreateWorld(&world_def);
}

void World::update() {
    float dt = 1.0f / 60.0f;
    const int sub_step_count = 4;
    b2World_Step(world, dt, sub_step_count);

    b2ContactEvents events = b2World_GetContactEvents(world);

    for (int i =0; i < events.hitCount; i++) {
        b2ContactHitEvent event = events.hitEvents[i];

        b2BodyId body_a = b2Shape_GetBody(event.shapeIdA);
        b2BodyId body_b = b2Shape_GetBody(event.shapeIdB);

        auto car_a = static_cast<Car*>(b2Body_GetUserData(body_a));
        auto car_b = static_cast<Car*>(b2Body_GetUserData(body_b));

        if (car_a && car_b) {
            float force = event.approachSpeed;
            b2Vec2 normal = event.normal;
            b2Vec2 vec_a = b2Body_GetLinearVelocity(body_a);
            b2Vec2 vec_b = b2Body_GetLinearVelocity(body_b);

            float vel = b2Dot(vec_a - vec_b, event.normal);

            if (vel > 0) {
                car_a->handle_hit(normal, force, true);
                car_b->handle_hit(normal, force, false);
            }
            else {
                car_a->handle_hit(normal, force, false);
                car_b->handle_hit(normal, force, true);
            }

        }
        else if (car_a) {
            car_a->handle_hit(event.normal, event.approachSpeed, false);
        }
    }
}

b2WorldId World::get_id() {
    return world;
}

void World::generate_map(std::string& map_name) {
    std::vector<StaticBody> boxes = parser.parse_map(map_name);

    create_collisions(boxes);
}

std::vector<GraphNode> World::generate_corners(std::string& map_name) {
    std::vector<Corner> corners = parser.parse_corners(map_name);
    return create_nodes(corners);
}

std::vector<GraphNode> World::create_nodes(std::vector<Corner>& corners) {
    std::vector<GraphNode> nodes;
    nodes.resize(corners.size());

    for (size_t i=0; i < corners.size(); i++) {
        nodes[i].x = corners[i].x;
        nodes[i].y = corners[i].y;

        for (size_t j =0; j < corners.size(); j++) {
            if ( i == j) continue;
            if (is_visible(corners[i], corners[j])) {
                float dx = corners[j].x - corners[i].x;
                float dy = corners[j].y - corners[i].y;
                float dist = dx*dx + dy*dy;
                nodes[i].neighbors.push_back(static_cast<int>(j));
                nodes[i].dist.push_back(dist);
            }
        }
    }

    return nodes;
}

bool World::is_visible(const Corner& _origin, const Corner& _traslation) const {
    b2Vec2 origin = {_origin.x, _origin.y};
    b2Vec2 traslation = {_traslation.x - origin.x, _traslation.y - origin.y};
    b2QueryFilter filter = b2DefaultQueryFilter();

    b2RayResult result = b2World_CastRayClosest(world, origin, traslation, filter);

    return result.hit == false;
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