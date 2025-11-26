//
// Created by vboxuser on 11/22/25.
//

#ifndef TALLER_TP_CAR_NPC_H
#define TALLER_TP_CAR_NPC_H

#include <box2d/box2d.h>

#include "src/common/car_DTO.h"

#include "parser_yaml.h"
#include "world.h"

class CarNPC {
    b2BodyId body_id;

    void rotate(GraphNode& target);

public:
    explicit CarNPC(GraphNode& start_corner, b2WorldId& world);
    void move(GraphNode& target);
    b2Vec2 get_position();
    b2Vec2 get_forward();
    void apply_friction();
    CarDTO get_state();
};


#endif  // TALLER_TP_CAR_NPC_H
