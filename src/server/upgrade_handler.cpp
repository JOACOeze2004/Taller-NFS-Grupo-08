#include "upgrade_handler.h"
#include "../common/constants.h"

UpgradeHandler::UpgradeHandler() { initialize_prices(); }

void UpgradeHandler::initialize_prices() {
    prices[ACCELERATION_UPGRADE] = std::chrono::seconds(ACCELERATION_PRICE);
    prices[HANDLING_UPGRADE] = std::chrono::seconds(HANDLING_PRICE);
    prices[NITRO_UPGRADE] = std::chrono::seconds(NITRO_PRICE);
    prices[LIFE_UPGRADE] = std::chrono::seconds(LIFE_PRICE);
    prices[BRAKE_UPGRADE] = std::chrono::seconds(BRAKE_PRICE);
    prices[MASS_UPGRADE] = std::chrono::seconds(MASS_PRICE);
}

std::pair<Upgrades, bool> UpgradeHandler::parse_command(int command_id) const {
    static const std::unordered_map<int, std::pair<Upgrades, bool>> command_map = {
        {SEND_ACCELERATION_UPGRADE, {ACCELERATION_UPGRADE, true}},
        {SEND_HANDLING_UPGRADE, {HANDLING_UPGRADE, true}},
        {SEND_NITRO_UPGRADE, {NITRO_UPGRADE, true}},
        {SEND_LIFE_UPGRADE, {LIFE_UPGRADE, true}},
        {SEND_BRAKE_UPGRADE, {BRAKE_UPGRADE, true}},
        {SEND_MASS_UPGRADE, {MASS_UPGRADE, true}},

        {SEND_ACCELERATION_DOWNGRADE, {ACCELERATION_UPGRADE, false}},
        {SEND_HANDLING_DOWNGRADE, {HANDLING_UPGRADE, false}},
        {SEND_NITRO_DOWNGRADE, {NITRO_UPGRADE, false}},
        {SEND_LIFE_DOWNGRADE, {LIFE_UPGRADE, false}},
        {SEND_BRAKE_DOWNGRADE, {BRAKE_UPGRADE, false}},
        {SEND_MASS_DOWNGRADE, {MASS_UPGRADE, false}}
    };
    
    auto it = command_map.find(command_id);
    if (it != command_map.end()) {
        return it->second;
    }

    return {NONE_UPGRADE, false};
}

bool UpgradeHandler::is_upgrade_command(int command_id) const { return parse_command(command_id).first != NONE_UPGRADE; }

void UpgradeHandler::apply_upgrade_to_car(int command_id, Car& car){
    auto [upgrade_type,is_upgrade]  = parse_command(command_id);
    if (upgrade_type != NONE_UPGRADE) {
        car.apply_upgrade(upgrade_type, is_upgrade);
    }
}

const std::map<Upgrades, std::chrono::seconds>& UpgradeHandler::get_prices() const { return prices; }


UpgradeHandler::~UpgradeHandler() {}