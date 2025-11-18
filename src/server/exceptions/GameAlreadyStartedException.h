#pragma once

#include <exception>
#include <string>

#define EXCEPTION_GAME_ALREADY_STARTED "The game has already started, you cannot join a game that has already started.try to join to other game."

class GameAlreadyStartedException: public std::exception {
public:
    const char* what() const noexcept override { return EXCEPTION_GAME_ALREADY_STARTED; }
};
