#ifndef TALLER_TP_WORLD_H
#define TALLER_TP_WORLD_H

#include <string>
#include "parser_yaml.h"
#include <box2d/box2d.h>

class World {
    b2WorldId world;
    ParserYaml parser;
    std::vector<b2BodyId> bodies;

private:
    void create_collisions(std::vector<StaticBody>& boxes);

public:
    explicit World();
    void update();
    b2WorldId get_id();
    void generate_map(std::string& map_name);
    ~World();
};


#endif  // TALLER_TP_WORLD_H
