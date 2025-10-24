#include "client_protocol.h"

ClientProtocol::ClientProtocol(Socket& _socket) : protocol(_socket) {  }


void ClientProtocol::send_message(const std::string& message) const {
    protocol.send_message(message);
}

std::string ClientProtocol::receive_message() const {
    return protocol.receive_message();
}

CarState ClientProtocol::receive_car_state() const {
    return protocol.receive_car_state();
}

void ClientProtocol::send_byte(const uint8_t byte) const {
    protocol.send_byte(byte);
}

uint8_t ClientProtocol::receive_byte() const{
    return protocol.receive_byte();
}

void ClientProtocol::close(){
    protocol.close_socket();
}