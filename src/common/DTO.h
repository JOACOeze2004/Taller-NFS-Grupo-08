#ifndef TALLER_TP_DTO_H
#define TALLER_TP_DTO_H

#include "car_DTO.h"
#include <unordered_map>

struct LobbyCarDTO {
    int car_id;
    float acceleration;
    float brake;
    float handling;
    float life;
    float mass;
};

enum StateRunning {
    IN_GAME,
    FINISHED,
    DEAD, 
    TIMEOUTED
};

enum Map {
    SAN_ANDREAS,
    VICE_CITY,
    LIBERTY_CITY
};

enum Upgrades {
    NONE_UPGRADE,
    SPEED,
    ACCELERATION_UPGRADE,
    HANDLING_UPGRADE,
    NITRO,
    LIFE
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
    float x;
    float y;
    float angle;
};

enum TypeCheckpoint{
    REGULAR,
    FINAL,
    FIRST
};

struct Snapshot { //deberia ser una clase
    int id;
    std::unordered_map<int, CarDTO> cars;
    enum StateRunning state;
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
    std::unordered_map<int, LobbyCarDTO> lobby_cars; // el int es el car_id. al cliente no le importa el int igual
    //std::unordered_map<int, playerDTO> players_info;

};

struct playerDTO {
    std::string name;
    float time;
    int position;
};

#endif  // TALLER_TP_DTO_H
