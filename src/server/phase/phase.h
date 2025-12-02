#ifndef TALLER_TP_PHASE_HANDLER_H
#define TALLER_TP_PHASE_HANDLER_H
#include "../client_command.h"
#include "../../common/DTO.h"
#include <chrono>

class Gameloop;

class Phase {
protected:
    Gameloop *gameloop;
    float duration;
    std::chrono::steady_clock::time_point start_time;

    int get_elapsed_ms() const;
    int calculate_remaining_time(float base_time, int elapsed_ms) const;
    int calculate_remaining_time_with_penalty(float base_time, int penalty_ms, int elapsed_ms) const;
    bool is_in_lobby() const;

public:
    explicit Phase(Gameloop* _gameloop, float _duration);
    
    void run();
    int get_time_remaining_ms(const float base_time, int player_id) const;
    int get_time_remaining_ms(const float base_time) const;
    int get_time() const;

    virtual State get_current_phase_state() const = 0;
    virtual bool should_continue() = 0;
    virtual void execute(ClientCommand& command) = 0;
    virtual void end() = 0;
    virtual void update(int time_ms) = 0;

    virtual ~Phase();
};


#endif  // TALLER_TP_PHASE_HANDLER_H
