#pragma once

#include <exception>
#include <string>

#define EXCEPTION_GAME_FULL "The game is full (8/8), try to join to other game."

class GameFullException: public std::exception {
public:
    const char* what() const noexcept override { return EXCEPTION_GAME_FULL; }
};
