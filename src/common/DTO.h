#ifndef TALLER_TP_DTO_H
#define TALLER_TP_DTO_H

#include <chrono>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include "car_DTO.h"
#include "upgrades.h"

struct LobbyCarDTO {
    int car_id;
    float acceleration;
    float brake;
    float handling;
    float life;
    float mass;
};

enum State{
    IN_LOBBY,
    IN_COUNTDOWN,
    IN_RACE,
    IN_WORK_SHOP,
    FINAL_RESULTS
};

enum Map {
    SAN_ANDREAS,
    VICE_CITY,
    LIBERTY_CITY
};

enum CollisionType {
    NONE_COLLISION,
    CHILL_COLLISION,
    HEAVY_COLLISION
    //ADD MORE COLLISSION TYPES IF NEEDED 
};

struct CheckpointCoords{
    float x;
    float y;
};

struct HintCoords{
    float x = 0;
    float y = 0;
    float angle = 0;
};

enum TypeCheckpoint{
    REGULAR,
    FINAL,
    FIRST
};

struct CarRacingInfo {
    std::string name;
    float time;
    int position;
};

struct Snapshot {
    int id;
    int game_id;
    std::unordered_map<int, CarDTO> cars;
    int position;
    int cars_count;
    enum Map map;
    enum Upgrades upgrade;
    bool upgradeable;
    enum CollisionType collision;
    CheckpointCoords checkpoint;
    HintCoords hint;
    enum TypeCheckpoint type_checkpoint;
    int time_ms;
    State state;
    std::unordered_map<int, LobbyCarDTO> lobby_cars;
    bool is_owner;
    std::map<Upgrades, std::chrono::seconds> prices;
    std::vector<CarRacingInfo> cars_finished;
    std::map<int, long long> player_total_times;
    int total_checkpoints; 
    int current_checkpoint;
    std::map<Upgrades, int> upgrades;
};

using FinalScoreList = std::vector<CarRacingInfo>;
#endif  // TALLER_TP_DTO_H
