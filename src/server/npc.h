#ifndef TALLER_TP_NPC_H
#define TALLER_TP_NPC_H
#include <vector>

#include <box2d/box2d.h>

#include "car_npc.h"
#include "parser_yaml.h"
#include "world.h"

class NPC {
    std::vector<GraphNode>* corners;
    size_t current_corner;
    int next_corner;
    b2WorldId world;
    CarNPC car;
    int state = 0;
    float reverse_timer = 1.0f;

    void choose_next_corner();

public:
    explicit NPC(b2WorldId world, std::vector<GraphNode>* _corners, int start_corner);
    void update();
    CarDTO get_state();
};


#endif  // TALLER_TP_NPC_H
