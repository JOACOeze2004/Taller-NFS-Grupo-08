#ifndef UPGRADE_HANDLER_H
#define UPGRADE_HANDLER_H

#include <map>
#include <chrono>
#include "../common/DTO.h"

#include "car.h"

class UpgradeHandler {
private:
    std::map<Upgrades, std::chrono::seconds> prices;
    
    void initialize_prices();
    std::pair<Upgrades, bool> parse_command(int command_id) const;

public:
    explicit UpgradeHandler();
    bool is_upgrade_command(int command_id) const;
    void apply_upgrade_to_car(int command_id, Car& car);
    const std::map<Upgrades, std::chrono::seconds>& get_prices() const;

    ~UpgradeHandler();
};


#endif
