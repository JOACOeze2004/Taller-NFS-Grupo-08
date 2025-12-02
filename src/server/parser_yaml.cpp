#include "parser_yaml.h"

#include <filesystem>

#include <yaml-cpp/yaml.h>

#include "../common/constants.h"
#include "CarStats.h"

ParserYaml::ParserYaml() : cars_file(YAML::LoadFile(MAPS_PATH)) {
    initialize_map_paths();
}


void ParserYaml::initialize_map_paths() {
    map_paths[LIBERTY_CITY_STR] = LIBERTY_CITY_PATH;
    map_paths[SAN_ANDREAS_STR] = SAN_ANDREAS_PATH;
    map_paths[VICE_CITY_STR] = VICE_CITY_PATH;
}

YAML::Node ParserYaml::load_map_file(const std::string& map_name) {
    auto it = map_paths.find(map_name);
    if (it == map_paths.end()) {
        throw std::invalid_argument(UNKNOWN_PATH);
    }
    return YAML::LoadFile(it->second);
}


std::string ParserYaml::get_map_id(const std::string& map_name) {
    if (map_name == LIBERTY_CITY_STR){
        return "liberty_city";
    } 
    if (map_name == SAN_ANDREAS_STR){
        return "san_andreas";
    } 
    return "vice_city";
}

StaticBody ParserYaml::parse_box(const YAML::Node& obj) {
    StaticBody box;
    box.id = obj["id"].as<int>();
    box.x = obj["x"].as<float>();
    box.y = obj["y"].as<float>();
    box.width = obj["width"].as<float>();
    box.height = obj["height"].as<float>();
    return box;
}

Corner ParserYaml::parse_corner(const YAML::Node& obj) {
    Corner corner;
    corner.x = obj["x"].as<float>();
    corner.y = obj["y"].as<float>();
    return corner;
}

Checkpoint ParserYaml::parse_checkpoint(const YAML::Node& cp) {
    Checkpoint c;
    c.x = cp["x"].as<float>();
    c.y = cp["y"].as<float>();
    c.order = cp["order"].as<int>();
    return c;
}

Hint ParserYaml::parse_hint(const YAML::Node& hnt) {
    Hint hint;
    hint.x = hnt["x"].as<float>();
    hint.y = hnt["y"].as<float>();
    hint.rotation = hnt["rotation"].as<float>();
    return hint;
}

std::vector<StaticBody> ParserYaml::parse_boxes_layer(const YAML::Node& map) {
    std::vector<StaticBody> boxes;

    for (const auto& layer : map["layers"]) {
        if (layer["name"].as<std::string>() != "BOXES"){
            continue;
        } 
        for (const auto& obj : layer["objects"]) {
            boxes.push_back(parse_box(obj));
        }
    }
    return boxes;
}

std::vector<Corner> ParserYaml::parse_corners_layer(const YAML::Node& map) {
    std::vector<Corner> corners;

    for (const auto& layer : map["layers"]) {
        if (layer["name"].as<std::string>() != "ESQUINAS"){
            continue;
        } 
        for (const auto& obj : layer["objects"]) {
            corners.push_back(parse_corner(obj));
        }
    }
    return corners;
}

std::vector<StaticBody> ParserYaml::parse_map(std::string& map_name) {
    YAML::Node map = load_map_file(map_name);
    return parse_boxes_layer(map);
}

std::vector<Corner> ParserYaml::parse_corners(std::string& map_name) {
    YAML::Node map = load_map_file(map_name);
    return parse_corners_layer(map);
}

CarStats ParserYaml::parse_car(const int car_id) {
    int normalized_id = car_id % 7;
    auto car_node = cars_file["cars_stats"][normalized_id];
    return CarStats{ car_node["mass"].as<float>(), car_node["handling"].as<float>(), car_node["acceleration"].as<float>(), car_node["braking"].as<float>()};
}

std::vector<Track> ParserYaml::parse_tracks(const std::string& tracks_dir, const std::string& map_name) {
    std::vector<Track> tracks;
    std::string map_id = get_map_id(map_name);
    for (const auto& file : std::filesystem::directory_iterator(tracks_dir)) {
        YAML::Node node = YAML::LoadFile(file.path());
        
        if (node["cityId"].as<std::string>() != map_id) {
            continue;
        }
        Track track;
        track.city_id = map_id;
        for (const auto& cp : node["checkpoints"]) {
            track.checkpoints.push_back(parse_checkpoint(cp));
        }
        for (const auto& hnt : node["hints"]) {
            track.hints.push_back(parse_hint(hnt));
        }   
        tracks.push_back(std::move(track));
    }
    return tracks;
}