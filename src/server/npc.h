#ifndef TALLER_TP_NPC_H
#define TALLER_TP_NPC_H
#include <vector>
#include <box2d/box2d.h>
#include "parser_yaml.h"
#include "car_npc.h"

class NPC {
    std::vector<Corner>* corners;
    size_t current_corner;
    int next_corner;
    b2WorldId world;
    CarNPC car;

    void choose_next_corner();
    bool is_visible(const Corner& _origin, const Corner& _traslation) const;
public:
    explicit NPC(b2WorldId world, std::vector<Corner>* _corners, int start_corner);
    void update();
};


#endif  // TALLER_TP_NPC_H
