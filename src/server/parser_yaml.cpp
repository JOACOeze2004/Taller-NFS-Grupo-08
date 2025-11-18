#include "parser_yaml.h"

#include <filesystem>

#include <yaml-cpp/yaml.h>

#include "CarStats.h"
#include "../common/constants.h"

ParserYaml::ParserYaml() : cars_file(YAML::LoadFile(MAPS_PATH)) {}

std::vector<StaticBody> ParserYaml::parse_map(std::string& map_name) {
    YAML::Node map;
    if (map_name == LIBERTY_CITY_STR ) {
        map = YAML::LoadFile(LIBERTY_CITY_PATH);
    }
    else if (map_name == SAN_ANDREAS_STR) {
        map = YAML::LoadFile(SAN_ANDREAS_PATH);
    }
    else if (map_name == VICE_CITY_STR) {
        map = YAML::LoadFile(VICE_CITY_PATH);
    }
    else {
        throw std::invalid_argument(UNKNOWN_PATH);
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

    return boxes;
}

CarStats ParserYaml::parse_car(const int car_id) {
    int _car_id = car_id % 7;
    auto id = cars_file["cars_stats"][_car_id];
    const auto mass = id["mass"].as<float>();
    const auto handling = id["handling"].as<float>();
    const auto acceleration = id["acceleration"].as<float>();
    const auto braking = id["braking"].as<float>();
    CarStats car_stats = {mass, handling, acceleration, braking};
    return car_stats;
}

std::vector<Track> ParserYaml::parse_tracks(const std::string& tracks_dir, const std::string& map_name) {
    std::vector<Track> tracks;
    std::string map_id;
    if (map_name == LIBERTY_CITY_STR) {
        map_id = "liberty_city";
    }
    else if (map_name == SAN_ANDREAS_STR) {
        map_id = "san_andreas";
    }
    else {
        map_id = "vice_city";
    }

    for (const auto& file : std::filesystem::directory_iterator(tracks_dir)) {
        std::string file_path = file.path();
        YAML::Node node = YAML::LoadFile(file_path);
        Track track;
        std::string city_id = node["cityId"].as<std::string>();
        if (city_id != map_id) {
            continue;
        }

        track.city_id = city_id;
        for (const auto& cp : node["checkpoints"]) {
            Checkpoint c;
            c.x = cp["x"].as<float>();
            c.y = cp["y"].as<float>();
            c.order = cp["order"].as<int>();
            track.checkpoints.push_back(c);
        }

        for (const auto& hnt : node["hints"]) {
            Hint hint;
            hint.x = hnt["x"].as<float>();
            hint.y = hnt["y"].as<float>();
            hint.rotation = hnt["rotation"].as<float>();
            track.hints.push_back(hint);
        }

        tracks.emplace_back(track);
    }

    return tracks;
}
