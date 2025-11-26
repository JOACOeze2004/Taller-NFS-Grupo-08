#include "sprite_loader.h"
#include <fstream>
#include <iostream>

bool SpriteLoader::fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

std::vector<SpriteData> SpriteLoader::loadCarSprites(const std::string& yaml_path) {
    std::vector<SpriteData> sprites;

    if (!fileExists(yaml_path)) {
        return sprites;
    }

    try {
        YAML::Node config = YAML::LoadFile(yaml_path);

        if (!config["cars"] || !config["cars"].IsSequence()) {
            return sprites;
        }

        for (const auto& car_node : config["cars"]) {
            SpriteData sprite;
            sprite.id = car_node["id"].as<int>();
            sprite.name = car_node["name"].as<std::string>();
            sprite.type = sprite.id;

            auto sprite_node = car_node["sprite"];
            sprite.rect.x = sprite_node["x"].as<int>();
            sprite.rect.y = sprite_node["y"].as<int>();
            sprite.rect.w = sprite_node["width"].as<int>();
            sprite.rect.h = sprite_node["height"].as<int>();

            sprites.push_back(sprite);
        }
    } catch (const YAML::Exception& e) {
        std::cerr << "[SPRITE_LOADER] Error parsing " << yaml_path
                  << ": " << e.what() << std::endl;
    }

    return sprites;
}

std::vector<SpriteData> SpriteLoader::loadCheckpointSprites(const std::string& yaml_path) {
    std::vector<SpriteData> sprites;
    if (!fileExists(yaml_path)) {
        return sprites;
    }
    try {
        YAML::Node config = YAML::LoadFile(yaml_path);

        if (!config["checkpoints"] || !config["checkpoints"].IsSequence()) {
            return sprites;
        }

        for (const auto& cp_node : config["checkpoints"]) {
            SpriteData sprite;
            sprite.id = cp_node["id"].as<int>();
            sprite.name = cp_node["name"].as<std::string>();
            sprite.type = cp_node["type"].as<int>();

            auto sprite_node = cp_node["sprite"];
            sprite.rect.x = sprite_node["x"].as<int>();
            sprite.rect.y = sprite_node["y"].as<int>();
            sprite.rect.w = sprite_node["width"].as<int>();
            sprite.rect.h = sprite_node["height"].as<int>();

            sprites.push_back(sprite);
        }
    } catch (const YAML::Exception& e) {
        std::cerr << "[SPRITE_LOADER] Error parsing " << yaml_path
                  << ": " << e.what() << std::endl;
    }
    return sprites;
}

SpriteData SpriteLoader::loadHintSprite(const std::string& yaml_path) {
    SpriteData sprite{};
    sprite.rect = {260, 20, 120, 150};
    if (!fileExists(yaml_path)) {
        return sprite;
    }
    try {
        YAML::Node config = YAML::LoadFile(yaml_path);

        if (config["hints"] && config["hints"].IsSequence() && config["hints"].size() > 0) {
            auto hint_node = config["hints"][0];
            sprite.id = hint_node["id"].as<int>();
            sprite.name = hint_node["name"].as<std::string>();

            auto sprite_node = hint_node["sprite"];
            sprite.rect.x = sprite_node["x"].as<int>();
            sprite.rect.y = sprite_node["y"].as<int>();
            sprite.rect.w = sprite_node["width"].as<int>();
            sprite.rect.h = sprite_node["height"].as<int>();
        }
    } catch (const YAML::Exception& e) {
        std::cerr << "[SPRITE_LOADER] Error parsing " << yaml_path
                  << ": " << e.what() << std::endl;
    }
    return sprite;
}

std::vector<UpgradeData> SpriteLoader::loadUpgradeSprites(const std::string& yaml_path) {
    std::vector<UpgradeData> upgrades;
    if (!fileExists(yaml_path)) {
        return upgrades;
    }
    try {
        YAML::Node config = YAML::LoadFile(yaml_path);
        if (!config["upgrades"] || !config["upgrades"].IsSequence()) {
            return upgrades;
        }
        for (const auto& upgrade_node : config["upgrades"]) {
            UpgradeData upgrade;
            upgrade.id = upgrade_node["id"].as<int>();
            upgrade.name = upgrade_node["name"].as<std::string>();
            upgrade.title = upgrade_node["title"].as<std::string>();
            upgrade.description = upgrade_node["description"].as<std::string>();
            upgrade.order = upgrade_node["order"].as<int>();

            auto sprite_node = upgrade_node["sprite"];
            upgrade.sprite.x = sprite_node["x"].as<int>();
            upgrade.sprite.y = sprite_node["y"].as<int>();
            upgrade.sprite.w = sprite_node["width"].as<int>();
            upgrade.sprite.h = sprite_node["height"].as<int>();

            auto buttons = upgrade_node["buttons"];
            upgrade.button_left = buttons["left"].as<std::string>();
            upgrade.button_right = buttons["right"].as<std::string>();

            upgrades.push_back(upgrade);
        }
    } catch (const YAML::Exception& e) {
        std::cerr << "[SPRITE_LOADER] Error parsing " << yaml_path
                  << ": " << e.what() << std::endl;
    }
    return upgrades;
}

ArrowData SpriteLoader::loadArrowSprite(const std::string& yaml_path) {
    ArrowData arrow{};
    arrow.sprite = {550, 340, 240, 230};
    if (!fileExists(yaml_path)) {
        return arrow;
    }
    try {
        YAML::Node config = YAML::LoadFile(yaml_path);

        if (config["arrows"] && config["arrows"]["sprite"]) {
            auto sprite_node = config["arrows"]["sprite"];
            arrow.sprite.x = sprite_node["x"].as<int>();
            arrow.sprite.y = sprite_node["y"].as<int>();
            arrow.sprite.w = sprite_node["width"].as<int>();
            arrow.sprite.h = sprite_node["height"].as<int>();
        }
    } catch (const YAML::Exception& e) {
        std::cerr << "[SPRITE_LOADER] Error parsing " << yaml_path
                  << ": " << e.what() << std::endl;
    }
    return arrow;
}