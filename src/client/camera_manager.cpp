#include "camera_manager.h"

CameraManager::CameraManager(int screen_w, int screen_h, float map_w, float map_h, float zoom)
    : camera_x(0.0f), camera_y(0.0f), camera_id(-1),
      map_width(map_w), map_height(map_h),
      screen_width(screen_w), screen_height(screen_h),
      zoom_factor(zoom) {}

void CameraManager::update( const std::unordered_map<int, CarDTO>& cars) {
    if (camera_id < 0 || cars.find(camera_id) == cars.end()) {
        return;
    }

    const CarDTO& camera_car = cars.at(camera_id);

    camera_x = (camera_car.x - static_cast<float>(screen_width) / (2.0f * zoom_factor));
    camera_y = (camera_car.y - static_cast<float>(screen_height) / (2.0f * zoom_factor));

    const float viewport_width = static_cast<float>(screen_width) / zoom_factor;
    const float viewport_height = static_cast<float>(screen_height) / zoom_factor;

    if (camera_x < 0.0f) camera_x = 0.0f;
    if (camera_y < 0.0f) camera_y = 0.0f;
    if (camera_x > map_width - viewport_width) 
        camera_x = map_width - viewport_width;
    if (camera_y > map_height - viewport_height) 
        camera_y = map_height - viewport_height;
}

void CameraManager::set_camera_id(int id) {
    camera_id = id;
}