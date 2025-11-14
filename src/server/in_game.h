#ifndef TALLER_TP_IN_GAME_H
#define TALLER_TP_IN_GAME_H
#include "phase.h"
#include "../common/DTO.h"
#include "race.h"
#include <chrono>
#include <thread>


class InGame : public Phase {
    std::chrono::steady_clock::time_point start_time;
    std::string& current_map;
    Race race;
    std::map<int, Car>* cars;


    void delete_deads();
    void set_start_race();
    void check_end_race();

public:
    explicit InGame(Gameloop* _gameloop, float _duration, std::string& map_name, const std::string& races_path, std::map<int, Car>* cars);
    void run() override;
    void execute() override;
    StateRunning get_state(const int& id);
    CheckpointCoords get_checkpoint(const int& id) const;
    HintCoords get_hint(const int& id) const;
    TypeCheckpoint get_cp_type(const int& id) const;
    int get_position(const int& id) const;
    void update_positions();
    int get_time_remaining_ms() const;
};


#endif  // TALLER_TP_IN_GAME_H
