#ifndef TALLER_TP_PROTOCOL_H
#define TALLER_TP_PROTOCOL_H

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <arpa/inet.h>

#include "../server/car_state.h"

#include "car_DTO.h"
#include "constants.h"
#include "socket.h"

class Protocol {
public:
    explicit Protocol(Socket& socket);
    void send_message(const std::string& message) const;
    std::string receive_message() const;

    void send_byte(const uint8_t byte) const;
    uint8_t receive_byte() const;

    void send_big_endian_16(const uint16_t value) const;
    uint16_t receive_big_endian_16() const;
    
    void send_big_endian_32(const uint32_t value) const;
    uint32_t receive_big_endian_32() const;

    void send_string(const std::string& str) const;
    std::string receive_string(size_t size) const;

    void send_float(const float value) const;
    float receive_float() const;

    void close_socket();
private:
    Socket& socket;
};

#endif  // TALLER_TP_PROTOCOL_H
