#ifndef TALLER_TP_DTO_H
#define TALLER_TP_DTO_H

#include "car_DTO.h"
#include <unordered_map>

struct DTO {
    int id;
    std::unordered_map<int, CarDTO> cars;
};

#endif  // TALLER_TP_DTO_H
