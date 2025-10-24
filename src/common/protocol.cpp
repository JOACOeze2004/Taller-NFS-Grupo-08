#include "protocol.h"

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

void Protocol::send_byte(const uint8_t byte) const {
    size_t sent = socket.sendall(&byte, sizeof(byte));
    if (sent != sizeof(byte)) {
        throw std::runtime_error("Byte not sent");
    }
}

uint8_t Protocol::receive_byte() const {
    uint8_t byte;
    socket.recvall(&byte, sizeof(byte));
    return byte;
}


void Protocol::send_big_endian_16(const uint16_t value) const{
    uint16_t big_endian_to_send = htons(value);
    size_t size_sent = socket.sendall(&big_endian_to_send, sizeof(big_endian_to_send));
    if (size_sent != sizeof(big_endian_to_send)) {
        throw std::runtime_error("Error: uint16 was not sent");
    }
}

uint16_t Protocol::receive_big_endian_16() const{
    uint16_t big_endian_to_receive;
    socket.recvall(&big_endian_to_receive, sizeof(big_endian_to_receive));
    return ntohs(big_endian_to_receive);
}

uint32_t Protocol::receive_big_endian_32() const{
    uint32_t big_endian_to_receive;
    socket.recvall(&big_endian_to_receive, sizeof(big_endian_to_receive));
    return ntohl(big_endian_to_receive);
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


void Protocol::send_car_state(const CarDTO& state) const {
    socket.sendall(&state.x, sizeof(state.x));
    socket.sendall(&state.y, sizeof(state.y));
    socket.sendall(&state.velocity, sizeof(state.velocity));
    socket.sendall(&state.angle, sizeof(state.angle));
}

CarDTO Protocol::receive_car_state() const {
    CarDTO state;
    socket.recvall(&state.x, sizeof(state.x));
    socket.recvall(&state.y, sizeof(state.y));
    socket.recvall(&state.velocity, sizeof(state.velocity));
    socket.recvall(&state.angle, sizeof(state.angle));
    return state;
}

void Protocol::close_socket(){
    try {
        socket.shutdown(SHUT_RDWR);
        socket.close();
    } catch(const std::exception& e){
        std::cerr << "Socket already closed in protocol: " << e.what() << '\n';
    }
}