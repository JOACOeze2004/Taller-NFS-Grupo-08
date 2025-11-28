#include "world.h"
#include "car.h"
#include <chrono>

std::unordered_map<std::string, std::vector<StaticBody>> World::cached_maps;
std::unordered_map<std::string, std::vector<Corner>> World::cached_corners;

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

    for (int i = 0; i < events.hitCount; i++) {
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
            } else {
                car_a->handle_hit(normal, force, false);
                car_b->handle_hit(normal, force, true);
            }
        } else if (car_a) {
            car_a->handle_hit(event.normal, event.approachSpeed, false);
        } else if (car_b) {
            car_b->handle_hit(event.normal, event.approachSpeed, false);
        }
    }
}

b2WorldId World::get_id() { return world; }

void World::generate_map(std::string& map_name) {
    auto it = cached_maps.find(map_name);
    std::vector<StaticBody> boxes;
    
    if (it != cached_maps.end()) {
        boxes = it->second;
    } else {
        boxes = parser.parse_map(map_name);
        cached_maps[map_name] = boxes;
    }
    create_collisions(boxes);
}

std::vector<GraphNode> World::generate_corners(std::string& map_name) {
    auto it = cached_corners.find(map_name);
    std::vector<Corner> corners;
    
    if (it != cached_corners.end()) {
        corners = it->second;
    } else {
        corners = parser.parse_corners(map_name);
        cached_corners[map_name] = corners;
    }
    
    auto result = create_nodes(corners);    
    return result;
}

std::vector<GraphNode> World::create_nodes(std::vector<Corner>& corners) {
    std::vector<GraphNode> nodes;
    nodes.resize(corners.size());
    float MAX_VISIBILITY_DISTANCE_SQ = 300.0f * 300.0f;
    size_t MAX_NEIGHBORS_PER_NODE = 12;
    
    int total_checks = 0;
    int skipped_by_distance = 0;
    int skipped_by_limit = 0;

    for (size_t i = 0; i < corners.size(); i++) {
        nodes[i].x = corners[i].x;
        nodes[i].y = corners[i].y;

        nodes[i].neighbors.reserve(MAX_NEIGHBORS_PER_NODE);
        nodes[i].dist.reserve(MAX_NEIGHBORS_PER_NODE);
        
        std::vector<std::pair<size_t, float>> candidates;
        candidates.reserve(corners.size());

        for (size_t j = 0; j < corners.size(); j++) {
            if (i == j){
                continue;
            }             
            total_checks++;            
            float dist_sq = calculate_distance_squared(corners[i], corners[j]);
            
            if (dist_sq > MAX_VISIBILITY_DISTANCE_SQ) {
                skipped_by_distance++;
                continue;
            }
            
            candidates.push_back({j, dist_sq});
        }
        std::sort(candidates.begin(), candidates.end(),[](const auto& a, const auto& b) { return a.second < b.second; });
        size_t checked = 0;
        for (const auto& [j, dist_sq] : candidates) {
            if (checked >= MAX_NEIGHBORS_PER_NODE) {
                skipped_by_limit++;
                continue;
            }
            
            if (is_visible(corners[i], corners[j])) {
                nodes[i].neighbors.push_back(static_cast<int>(j));
                nodes[i].dist.push_back(dist_sq);
                checked++;
            }
        }
    }
    return nodes;
}

float World::calculate_distance_squared(const Corner& from, const Corner& to) const {
    float dx = to.x - from.x;
    float dy = to.y - from.y;
    return dx * dx + dy * dy;
}

bool World::is_visible(const Corner& _origin, const Corner& _traslation) const {
    b2Vec2 origin = {_origin.x, _origin.y};
    b2Vec2 traslation = {_traslation.x - origin.x, _traslation.y - origin.y};
    b2QueryFilter filter = b2DefaultQueryFilter();

    b2RayResult result = b2World_CastRayClosest(world, origin, traslation, filter);

    return result.hit == false;
}

void World::create_collisions(std::vector<StaticBody>& boxes) {
    bodies.reserve(boxes.size());    
    for (const auto& box : boxes) {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_staticBody;
        bodyDef.position = {(box.x + box.width / 2), (box.y + box.height / 2)};

        b2BodyId body = b2CreateBody(world, &bodyDef);
        b2Body_EnableContactEvents(body, true);
        b2Body_EnableHitEvents(body, true);

        b2Polygon b2box = b2MakeBox(box.width / 2, box.height / 2);
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