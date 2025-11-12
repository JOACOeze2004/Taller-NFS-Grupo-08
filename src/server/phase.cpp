#include "phase.h"

Phase::Phase(Gameloop* _gameloop, float _duration) : gameloop(_gameloop), duration(_duration), cont(true) {}

bool Phase::should_continue() { return cont; }
