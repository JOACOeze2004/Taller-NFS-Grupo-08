#include "server_protocol.h"

ServerProtocol::ServerProtocol(Socket& _socket) : protocol(_socket) {  }


void ServerProtocol::send_message(const std::string& message) const {
    protocol.send_message(message);
}

std::string ServerProtocol::receive_message() const {
    return protocol.receive_message();
}

uint8_t ServerProtocol::receive_standar_command() const {
    return protocol.receive_byte();
}

void ServerProtocol::send_car_state(CarDTO& car){
    protocol.send_car_state(car);
}

void ServerProtocol::close(){
    protocol.close_socket();    
}