#ifndef TALLER_TP_CAR_DTO_H
#define TALLER_TP_CAR_DTO_H

enum StateRunning {
    IN_GAME,
    FINISHED,
    DEAD,
    TIMEOUTED
};

struct CarDTO {
    float x;
    float y;
    float velocity;
    float angle;
    int car_id;
    bool under_bridge;
    int life;
    bool nitro;
    float remaining_nitro;
    enum StateRunning state;
};

#endif  // TALLER_TP_CAR_DTO_H
