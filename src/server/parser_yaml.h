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

struct Corner {
    float x;
    float y;
};

struct Checkpoint {
    float x;
    float y;
    int order;
};

struct Hint {
    float x;
    float y;
    float rotation;
};

struct Track {
    std::string city_id;
    std::vector<Checkpoint> checkpoints;
    std::vector<Hint> hints;
};

class ParserYaml {
    YAML::Node cars_file;
public:
    explicit ParserYaml();
    std::vector<StaticBody> parse_map(std::string& map_name);
    std::vector<Corner> parse_corners(std::string& map_name);
    CarStats parse_car(const int car_id);
    std::vector<Track> parse_tracks(const std::string& tracks_dir, const std::string& map_name);
};


#endif  // TALLER_TP_PARSER_YAML_H
