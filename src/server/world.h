#ifndef TALLER_TP_WORLD_H
#define TALLER_TP_WORLD_H

#include <string>
#include "parser_yaml.h"
#include <box2d/box2d.h>
#include <map>

struct GraphNode {
    std::vector<int> neighbors;
    std::vector<float> dist;
    float x;
    float y;
};

class World {
    b2WorldId world;
    ParserYaml parser;
    std::vector<b2BodyId> bodies;

private:
    void create_collisions(std::vector<StaticBody>& boxes);
    void create_corners(std::vector<Corner>& corners);
    std::vector<GraphNode> create_nodes(std::vector<Corner>& corners);
    bool is_visible(const Corner& _origin, const Corner& _traslation) const;

public:
    explicit World();
    void update();
    b2WorldId get_id();
    void generate_map(std::string& map_name);
    std::vector<GraphNode> generate_corners(std::string& map_name);
    ~World();
};


#endif  // TALLER_TP_WORLD_H
