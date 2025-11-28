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
    std::vector<Track> tracks;
    std::map<int, Car> *cars;
    Track track;
    std::map<int, int> car_next_cp;
    std::map<int, int> car_next_hint;
    std::map<int, bool> finished;
    std::map<int, int> positions;
    std::vector<int> positions_order;

    int get_checkpoints(int id) const;
    float get_distance(const int id) const;
    bool compare_cars(int a, int b) const;


public:
    explicit Race(const std::string& track_name, std::map<int, Car> *_cars, const std::string& tracks_directory);
	bool car_finished(const int& id);
	bool car_dead(const int& id) const;
	void update_checkpoints();
    void update_positions_order();
    int get_position(const int& id) const;
    StateRunning get_state(const int& id, const int& time_remaining);
    CheckpointCoords get_checkpoint(const int& id) const;
    HintCoords get_hint(const int& id) const;
    TypeCheckpoint get_cp_type(const int& id) const;
    Checkpoint get_start() const;
    Checkpoint get_start_angle() const;
    void reset_race();
    void update_cars();
    bool all_cars_done();
    void spawn_cars();
    void pass_to_next_checkpoint(int id); 
    int get_checkpoint_amount() const;
    int get_current_checkpoint(const int id) const; 


    void activate_win(int& id);
};


#endif  // TALLER_NFS_GRUPO_08_RACE_H
