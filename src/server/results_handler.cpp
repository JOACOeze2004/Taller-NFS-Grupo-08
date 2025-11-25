#include "results_handler.h"
#include <ranges>
#include <iostream>

void ResultsHandler::initialize_player(const int id){
    player_total_times[id] = 0;
}

bool ResultsHandler::add_finished(const int id, const std::string& name, int time, int position) {
    if (is_player_in_finished(name)) {
        return false;
    }
    
    finished.emplace_back(name, time, position);
    player_total_times[id] += time;
    return true;
}

bool ResultsHandler::add_dead(const int id, const std::string& name, int time, int max_time) {
    if (is_player_in_deads(name)) {
        return false;
    }
    
    deads.emplace_back(name, time, -1);
    player_total_times[id] += max_time;
    return true;
}

bool ResultsHandler::is_player_in_finished(const std::string& name) const {
    return std::ranges::any_of(finished,[&](const CarRacingInfo& p) { return p.name == name; });
}

bool ResultsHandler::is_player_in_deads(const std::string& name) const {
    return std::ranges::any_of(deads,[&](const CarRacingInfo& p) { return p.name == name; });
}

FinalScoreList ResultsHandler::calculate_final_scores(const std::unordered_map<int, std::string>& names) const {
    FinalScoreList results;
    for (const auto& [id, total_time] : player_total_times) {
        results.push_back({names.at(id), static_cast<float>(total_time), 0});
    }
    std::ranges::sort(results, [](const auto& a, const auto& b) { return a.time < b.time;});
    for (size_t i = 0; i < results.size(); ++i) {
        results[i].position = static_cast<int>(i + 1);
    }
    return results;
}

void ResultsHandler::reset_for_next_race() {
    finished.clear();
    deads.clear();
}

void ResultsHandler::reset_all() {
    finished.clear();
    deads.clear();
    player_total_times.clear();
}

const std::vector<CarRacingInfo>& ResultsHandler::get_finished() const { return finished; }

const std::vector<CarRacingInfo>& ResultsHandler::get_deads() const { return deads; }

const std::map<int, long long>& ResultsHandler::get_total_times() const { return player_total_times; }

void ResultsHandler::subtract_upgrade_time(int player_id, int ms) {
    if (player_total_times.find(player_id) != player_total_times.end()) {
        player_total_times[player_id] -= ms;
        if (player_total_times[player_id] < 0) {
            player_total_times[player_id] = 0;
        }
    }
}

void ResultsHandler::add_upgrade_time(int player_id, int ms) { player_total_times[player_id] += ms; }