#pragma once

#include <exception>
#include <string>

#define EXCEPTION_INVALID_NAME "Invalid player name: "

class InvalidPlayerNameException: public std::exception {
    std::string msg;
public:
    InvalidPlayerNameException(const std::string& reason)
        : msg(EXCEPTION_INVALID_NAME + reason) {}
    const char* what() const noexcept override { return msg.c_str(); }
};
