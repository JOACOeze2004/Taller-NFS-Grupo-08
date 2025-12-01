#include "config.h"
#include <yaml-cpp/yaml.h>

Config& Config::instance() {
    static Config cfg;
    return cfg;
}

void Config::load(const std::string& yamlPath) {
    YAML::Node root = YAML::LoadFile(yamlPath);

    auto g = root["game"];
    game.min_players = g["min_players"].as<size_t>();
    game.max_players = g["max_players"].as<size_t>();
    game.max_races = g["max_races"].as<int>();
    game.race_start_time_ms = g["race_start_time_ms"].as<int>();
    game.max_time_per_race_ms = g["max_time_per_race_ms"].as<int>();
    game.max_npcs = g["max_npcs"].as<int>();

    auto c = root["car"];
    car.max_speed = c["max_speed"].as<float>();
    car.max_life = c["max_life"].as<float>();
    car.max_nitro = c["max_nitro"].as<float>();

    car.acceleration_upgrade_factor = c["acceleration_upgrade_factor"].as<float>();
    car.handling_upgrade_factor = c["handling_upgrade_factor"].as<float>();
    car.nitro_upgrade_factor = c["nitro_upgrade_factor"].as<float>();
    car.life_upgrade_factor = c["life_upgrade_factor"].as<float>();
    car.brake_upgrade_factor = c["brake_upgrade_factor"].as<float>();
    car.mass_upgrade_factor = c["mass_upgrade_factor"].as<float>();

    car.nitro_consumption = c["nitro_consumption"].as<int>();
    car.nitro_recharge_rate = c["nitro_recharge_rate"].as<int>();

    car.boost_factor = c["boost_factor"].as<float>();

    car.hitter_factor = c["hitter_factor"].as<float>();
    car.hitted_factor = c["hitted_factor"].as<float>();

    car.spawn_position = c["spawn_position"].as<std::vector<float>>();

    auto w = root["workshop"];
    workshop.acceleration_price = w["acceleration_price"].as<int>();
    workshop.handling_price = w["handling_price"].as<int>();
    workshop.nitro_price = w["nitro_price"].as<int>();
    workshop.life_price = w["life_price"].as<int>();
    workshop.mass_price = w["mass_price"].as<int>();
    workshop.brake_price = w["brake_price"].as<int>();
}