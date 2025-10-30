#ifndef TALLER_TP_WORLD_H
#define TALLER_TP_WORLD_H

#include <box2d/box2d.h>

class World {
    b2WorldId world_id;


public:
    explicit World();
    void update();
    b2WorldId get_id();
    ~World();
};


#endif  // TALLER_TP_WORLD_H
