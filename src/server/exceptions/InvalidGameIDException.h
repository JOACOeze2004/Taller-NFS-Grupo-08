#pragma once

#include <exception>
#include <string>

#define EXCEPTION_INVALID_ID "The game ID you're trying to join doesn't exist. Please try one that does."

class InvalidGameIDException: public std::exception {
public:
    const char* what() const noexcept override { return EXCEPTION_INVALID_ID; }
};
