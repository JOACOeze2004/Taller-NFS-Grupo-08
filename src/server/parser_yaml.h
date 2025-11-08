#ifndef TALLER_TP_PARSER_YAML_H
#define TALLER_TP_PARSER_YAML_H
#include <string>
#include <vector>

#include <yaml-cpp/node/node.h>
#include "CarStats.h"

struct StaticBody {
    int id;
    float x;
    float y;
    float width;
    float height;
};

class ParserYaml {
    YAML::Node cars_file;
public:
    explicit ParserYaml();
    std::vector<StaticBody> parse_map(std::string& map_name);
    CarStats parse_car(const int car_id);
};


#endif  // TALLER_TP_PARSER_YAML_H
