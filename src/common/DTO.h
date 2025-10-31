#ifndef TALLER_TP_DTO_H
#define TALLER_TP_DTO_H

#include "car_DTO.h"
#include <unordered_map>
#include <vector>

struct DTO {
    int id;
    std::unordered_map<int, CarDTO> cars;
};

struct Snapshot { //deberia ser una clase
    enum class StateRunning state;
    int position;
    int cars_count;
    int id;
    std::vector<CarDTO> cars; // capaz es un map
    enum class Map map;
    enum class Upgrades upgrade;
    bool upgradeable;
    enum class CollisionType collision;
    bool under_bridge;
    CheckpointCoords checkpoint;
    enum class TypeCheckpoint type_checkpoint;
};

enum class StateRunning {
    IN_GAME,
    FINISHED,
    DEAD, 
    TIMEOUTED
};

enum class Map {
    SAN_ANDREAS,
    VICE_CITY,
    LIBERTY_CITY
};

enum class Upgrades {
    NONE,
    SPEED,
    ACCELERATION,
    HANDLING,
    NITRO,
    LIFE
};

enum class CollisionType {
    NONE,
    CHILL_COLLISION,
    HEAVY_COLLISION
    //ADD MORE COLLISSION TYPES IF NEEDED 
};

struct CheckpointCoords{
    float x;
    float y;
};

enum class TypeCheckpoint{
    REGULAR,
    FINAL,
    FIRST
};

#endif  // TALLER_TP_DTO_H
