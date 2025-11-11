#ifndef TALLER_NFS_GRUPO_08_RACE_H
#define TALLER_NFS_GRUPO_08_RACE_H
#include <algorithm>
#include <map>
#include <string>

#include "src/common/DTO.h"

#include "car.h"
#include "parser_yaml.h"


class Race {
    ParserYaml parser;
    Track track;
    std::map<int, Car> *cars;
    std::map<int, int> car_next_cp;
    std::map<int, int> car_next_hint;
    std::map<int, bool> finished;
    std::map<int, int> positions;
    std::vector<int> positions_order;

    int get_checkpoints(int id) const;
    float get_distance(const int id) const;
    bool compare_cars(int a, int b) const;


public:
    explicit Race(const std::string& track_name, std::map<int, Car> *_cars);
	bool car_finished(const int& id);
	bool car_dead(const int& id) const;
	void update_checkpoints();
    void update_positions_order();
    int get_position(const int& id) const;
    StateRunning get_state(const int& id, const int& time_remaining);
    CheckpointCoords get_checkpoint(const int& id) const;
    HintCoords get_hint(const int& id) const;
    TypeCheckpoint get_cp_type(const int& id) const;
};


#endif  // TALLER_NFS_GRUPO_08_RACE_H
