#include "parser_yaml.h"
#include <yaml-cpp/yaml.h>

std::vector<StaticBody> ParserYaml::parse_map(std::string& map_name) {
    YAML::Node map;
    if (map_name == "Liberty City") {
        map = YAML::LoadFile("../src/server/libertycity.yaml");
    }
    else if (map_name == "San Andreas") {
        map = YAML::LoadFile("../src/server/sanandreas.yaml");
    }
    else if (map_name == "Vice City") {
        map = YAML::LoadFile("../src/server/vicecity.yaml");
    }
    else {
        throw std::invalid_argument("Unknown map name");
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
