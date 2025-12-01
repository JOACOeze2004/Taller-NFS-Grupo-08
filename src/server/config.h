#ifndef TALLER_TP_CONFIG_H
#define TALLER_TP_CONFIG_H

#include <string>
#include <vector>

struct GameConfig {
    size_t min_players;
    size_t max_players;
    int max_races;
    int race_start_time_ms;
    int max_time_per_race_ms;
    int max_npcs;
};

struct CarConfig {
    float max_speed;
    float max_life;
    float max_nitro;

    float acceleration_upgrade_factor;
    float handling_upgrade_factor;
    float nitro_upgrade_factor;
    float life_upgrade_factor;
    float brake_upgrade_factor;
    float mass_upgrade_factor;

    int nitro_consumption;
    int nitro_recharge_rate;

    float boost_factor;

    float hitter_factor;
    float hitted_factor;

    std::vector<float> spawn_position;
};

struct WorkshopConfig {
    int acceleration_price;
    int handling_price;
    int nitro_price;
    int life_price;
    int mass_price;
    int brake_price;
};

class Config {
public:
    static Config& instance();
    void load(const std::string& yamlPath);

    GameConfig game;
    CarConfig car;
    WorkshopConfig workshop;

private:
    Config() = default;
};

#endif  // TALLER_TP_CONFIG_H
