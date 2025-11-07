#ifndef TALLER_TP_CAR_DTO_H
#define TALLER_TP_CAR_DTO_H

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
};

#endif  // TALLER_TP_CAR_DTO_H
