#include "protocol.h"
#include <cstring>
#include "communicationEndedException.h"

Protocol::Protocol(Socket& socket)
    : socket(socket) {}

void Protocol::send_byte(const uint8_t byte) const {
    size_t sent = socket.sendall(&byte, sizeof(byte));
    if (sent != sizeof(byte)) {
        throw std::runtime_error("Byte not sent");
    }
}

uint8_t Protocol::receive_byte() const {
    uint8_t byte;
    int n = socket.recvall(&byte, sizeof(byte));
    if (n == 0) {
        throw CommunicationEndedException();
    }
    return byte;
}


void Protocol::send_big_endian_16(const uint16_t value) const{
    uint16_t big_endian_to_send = htons(value);
    size_t size_sent = socket.sendall(&big_endian_to_send, sizeof(big_endian_to_send));
    if (size_sent != sizeof(big_endian_to_send)) {
        throw std::runtime_error("Error: uint16 was not sent");
    }
}

void Protocol::send_big_endian_32(const uint32_t value) const{
    uint32_t big_endian_to_send = htonl(value);
    size_t size_sent = socket.sendall(&big_endian_to_send, sizeof(big_endian_to_send));
    if (size_sent != sizeof(big_endian_to_send)) {
        throw std::runtime_error("Error: uint32 was not sent");
    }
}

uint16_t Protocol::receive_big_endian_16() const{
    uint16_t big_endian_to_receive;
    int n = socket.recvall(&big_endian_to_receive, sizeof(big_endian_to_receive));
    if (n == 0) {
        throw CommunicationEndedException();
    }    
    return ntohs(big_endian_to_receive);
}

uint32_t Protocol::receive_big_endian_32() const{
    uint32_t big_endian_to_receive;
    int n = socket.recvall(&big_endian_to_receive, sizeof(big_endian_to_receive));
    if (n == 0) {
        throw CommunicationEndedException();
    }
    return ntohl(big_endian_to_receive);
}

void Protocol::send_big_endian_64(const uint64_t value) const {
    uint64_t big_endian = htobe64(value);
    socket.sendall(&big_endian, sizeof(big_endian));
}

uint64_t Protocol::receive_big_endian_64() const {
    uint64_t big_endian;
    socket.recvall(&big_endian, sizeof(big_endian));
    return be64toh(big_endian);
}

void Protocol::send_string(const std::string& str) const {
    size_t sent = socket.sendall(str.c_str(), str.size());
    if (sent != str.size()) {
        throw std::runtime_error("Error: The entire string was not sent.");
    }
}

std::string Protocol::receive_string(size_t size) const{
    std::vector<char> buffer(size);
    socket.recvall(buffer.data(), size);
    return std::string(buffer.begin(), buffer.end());
}

void Protocol::send_float(const float value) const {
    uint32_t parsed_value;
    std::memcpy(&parsed_value, &value, sizeof(float));
    parsed_value = htonl(parsed_value);
    socket.sendall(&parsed_value, sizeof(parsed_value));
}

float Protocol::receive_float() const {
    uint32_t parsed_value;
    int n = socket.recvall(&parsed_value, sizeof(parsed_value));
    if (n == 0) {
        throw CommunicationEndedException();
    }
    parsed_value = ntohl(parsed_value);
    float value;
    std::memcpy(&value, &parsed_value, sizeof(float));
    return value;
}

void Protocol::send_bool(const bool value) const { this->send_byte(value ? 1 : 0); }

bool Protocol::receive_bool() const{
    uint8_t byte = this->receive_byte();
    return byte != 0;
}

void Protocol::close_socket(){
    try {
        if (!socket.is_stream_recv_closed()){
            socket.shutdown(SHUT_RDWR);
        } if (!socket.is_stream_send_closed()){
            socket.shutdown(SHUT_RDWR);
        }        
        socket.close();
    } catch(const std::exception& e){ }
}