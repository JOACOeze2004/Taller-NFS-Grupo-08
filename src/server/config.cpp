#include "config.h"
#include <yaml-cpp/yaml.h>

Config& Config::instance() {
    static Config cfg;
    return cfg;
}

void Config::load(const std::string& yamlPath) {
    YAML::Node root = YAML::LoadFile(yamlPath);

    auto game_node = root["game"];
    game.min_players = game_node["min_players"].as<size_t>();
    game.max_players = game_node["max_players"].as<size_t>();
    game.max_races = game_node["max_races"].as<int>();
    game.race_start_time_ms = game_node["race_start_time_ms"].as<int>();
    game.max_time_per_race_ms = game_node["max_time_per_race_ms"].as<int>();
    game.max_npcs = game_node["max_npcs"].as<int>();

    auto car_node = root["car"];
    car.max_speed = car_node["max_speed"].as<float>();
    car.max_life = car_node["max_life"].as<float>();
    car.max_nitro = car_node["max_nitro"].as<float>();

    car.acceleration_upgrade_factor = car_node["acceleration_upgrade_factor"].as<float>();
    car.handling_upgrade_factor = car_node["handling_upgrade_factor"].as<float>();
    car.nitro_upgrade_factor = car_node["nitro_upgrade_factor"].as<float>();
    car.life_upgrade_factor = car_node["life_upgrade_factor"].as<float>();
    car.brake_upgrade_factor = car_node["brake_upgrade_factor"].as<float>();
    car.mass_upgrade_factor = car_node["mass_upgrade_factor"].as<float>();

    car.nitro_consumption = car_node["nitro_consumption"].as<int>();
    car.nitro_recharge_rate = car_node["nitro_recharge_rate"].as<int>();

    car.boost_factor = car_node["boost_factor"].as<float>();

    car.hitter_factor = car_node["hitter_factor"].as<float>();
    car.hitted_factor = car_node["hitted_factor"].as<float>();

    car.spawn_position = car_node["spawn_position"].as<std::vector<float>>();

    auto workshop_node = root["workshop"];
    workshop.acceleration_price = workshop_node["acceleration_price"].as<int>();
    workshop.handling_price = workshop_node["handling_price"].as<int>();
    workshop.nitro_price = workshop_node["nitro_price"].as<int>();
    workshop.life_price = workshop_node["life_price"].as<int>();
    workshop.mass_price = workshop_node["mass_price"].as<int>();
    workshop.brake_price = workshop_node["brake_price"].as<int>();
}