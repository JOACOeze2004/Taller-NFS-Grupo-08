
#ifndef TALLER_TP_CAMARA_MANAGER_H
#define TALLER_TP_CAMARA_MANAGER_H

#include <unordered_map>
#include "../common/DTO.h"

class CameraManager {
private:
    float camera_x;
    float camera_y;
    int camera_id;
    float map_width;
    float map_height;
    int screen_width;
    int screen_height;
    float zoom_factor;

public:
    CameraManager(int screen_w, int screen_h, float map_w, float map_h, float zoom);

    void update( const std::unordered_map<int, CarDTO>& cars);
    void set_camera_id(int id);

    float get_camera_x() const { return camera_x; }
    float get_camera_y() const { return camera_y; }
    int get_camera_id() const { return camera_id; }
};
#endif  // TALLER_TP_CAMARA_MANAGER_H
