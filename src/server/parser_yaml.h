#ifndef TALLER_TP_PARSER_YAML_H
#define TALLER_TP_PARSER_YAML_H
#include <string>
#include <vector>
#include <unordered_map>

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
public:
    explicit ParserYaml();
    std::vector<StaticBody> parse_map(std::string& map_name);
    std::vector<Corner> parse_corners(std::string& map_name);
    CarStats parse_car(const int car_id);
    std::vector<Track> parse_tracks(const std::string& tracks_dir, const std::string& map_name);

private:
    YAML::Node cars_file;
    std::unordered_map<std::string, std::string> map_paths;    
    
    YAML::Node load_map_file(const std::string& map_name);
    std::string get_map_id(const std::string& map_name);
    
    StaticBody parse_box(const YAML::Node& obj);
    Corner parse_corner(const YAML::Node& obj);
    
    std::vector<StaticBody> parse_boxes_layer(const YAML::Node& map);
    std::vector<Corner> parse_corners_layer(const YAML::Node& map);
    Checkpoint parse_checkpoint(const YAML::Node& cp);
    Hint parse_hint(const YAML::Node& hnt);
    
    void initialize_map_paths();
};


#endif  // TALLER_TP_PARSER_YAML_H
