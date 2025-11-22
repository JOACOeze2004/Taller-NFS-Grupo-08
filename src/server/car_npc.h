//
// Created by vboxuser on 11/22/25.
//

#ifndef TALLER_TP_CAR_NPC_H
#define TALLER_TP_CAR_NPC_H

#include <box2d/box2d.h>

#include "src/common/car_DTO.h"

#include "parser_yaml.h"

class CarNPC {
    b2BodyId body_id;

    void rotate(Corner& target);

public:
    explicit CarNPC(Corner& start_corner, b2WorldId& world);
    void move(Corner& target);
    b2Vec2 get_position();
    CarDTO get_state();
};


#endif  // TALLER_TP_CAR_NPC_H
