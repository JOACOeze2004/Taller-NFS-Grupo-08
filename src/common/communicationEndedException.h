#pragma once

#include <exception>
#include <string>

#define EXCEPTION_MESSAGE "Communication has ended because the socket has been closed"

class CommunicationEndedException: public std::exception {
public:
    const char* what() const noexcept override { return EXCEPTION_MESSAGE; }
};

