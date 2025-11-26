#ifndef TALLER_NFS_GRUPO_08_RESULTS_HANDLER_H
#define TALLER_NFS_GRUPO_08_RESULTS_HANDLER_H

#include <vector>
#include <map>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "../common/DTO.h"

class ResultsHandler{
private:
    std::vector<CarRacingInfo> finished;
    std::vector<CarRacingInfo> deads;
    std::map<int, long long> player_total_times;
public:
    ResultsHandler() = default;
    ~ResultsHandler() = default;

    void initialize_player(const int id);
    bool add_finished(const int id, const std::string& name, int time, int position);
    bool add_dead(const int id, const std::string& name, int time, int max_time);
    bool is_player_in_finished(const std::string& name) const;
    bool is_player_in_deads(const std::string& name) const;
    
    FinalScoreList calculate_final_scores(const std::unordered_map<int, std::string>& names) const;
    void reset_for_next_race();
    void reset_all();
    
    const std::vector<CarRacingInfo>& get_finished() const;
    const std::vector<CarRacingInfo>& get_deads() const;
    const std::map<int, long long>& get_total_times() const;

    void add_upgrade_time(int player_id, int ms);
    void subtract_upgrade_time(int player_id, int ms);
};


#endif  // TALLER_NFS_GRUPO_08_RESULTS_HANDLER_H