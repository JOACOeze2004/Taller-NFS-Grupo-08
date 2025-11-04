#ifndef TALLER_TP_DTO_H
#define TALLER_TP_DTO_H

#include "car_DTO.h"
#include <unordered_map>
#include <vector>

struct DTO {
    int id;
    std::unordered_map<int, CarDTO> cars;
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

enum TypeCheckpoint{
    REGULAR,
    FINAL,
    FIRST
};

struct Snapshot { //deberia ser una clase
    enum StateRunning state;
    int position;
    int cars_count;
    int id;
    std::vector<CarDTO> cars; // capaz es un map
    enum Map map;
    enum Upgrades upgrade;
    bool upgradeable;
    enum CollisionType collision;
    CheckpointCoords checkpoint;
    enum TypeCheckpoint type_checkpoint;
    int time_ms;
    //ver como manejar el tema de coches q ya no tienen que dibujarse
};



#endif  // TALLER_TP_DTO_H
