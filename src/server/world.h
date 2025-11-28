#ifndef TALLER_TP_WORLD_H
#define TALLER_TP_WORLD_H

#include <string>
#include <map>
#include <iostream>
#include <vector>

#include "parser_yaml.h"
#include <box2d/box2d.h>

struct GraphNode {
    std::vector<int> neighbors;
    std::vector<float> dist;
    float x;
    float y;
};

class World {
    private:

    b2WorldId world;
    ParserYaml parser;
    std::vector<b2BodyId> bodies;

    static std::unordered_map<std::string, std::vector<StaticBody>> cached_maps;
    static std::unordered_map<std::string, std::vector<Corner>> cached_corners;

    void create_collisions(std::vector<StaticBody>& boxes);
    void create_corners(std::vector<Corner>& corners);
    std::vector<GraphNode> create_nodes(std::vector<Corner>& corners);
    bool is_visible(const Corner& _origin, const Corner& _traslation) const;
    float calculate_distance_squared(const Corner& from, const Corner& to) const;


public:
    explicit World();
    void update();
    b2WorldId get_id();
    void generate_map(std::string& map_name);
    std::vector<GraphNode> generate_corners(std::string& map_name);
    ~World();
};



#endif  // TALLER_TP_WORLD_H
