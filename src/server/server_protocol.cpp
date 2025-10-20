#include "server_protocol.h"

ServerProtocol::ServerProtocol(Socket& _socket) : protocol(_socket) {  }


void ServerProtocol::send_message(const std::string& message) const {
    protocol.send_message(message);
}

std::string ServerProtocol::receive_message() const {
    return protocol.receive_message();
}

void ServerProtocol::close(){
    protocol.close_socket();    
}