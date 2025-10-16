#include "protocol.h"
#include <iostream>

Protocol::Protocol(Socket& socket)
    : socket(socket) {}

void Protocol::send_message(const std::string& message) const {
    const std::string msg = message + "\n";
    socket.sendall(msg.c_str(), msg.size());
}

std::string Protocol::receive_message() const {
    std::string result;
    char c;
    while (true) {
        if (const int bytes = socket.recvall(&c, 1); bytes <= 0) break;
        if (c == '\n') break;
        result.push_back(c);
    }

    return result;
}