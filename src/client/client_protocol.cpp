#include "client_protocol.h"


ClientProtocol::ClientProtocol(Socket& _socket) : protocol(_socket) {  }


void ClientProtocol::send_message(const std::string& message) const {
    protocol.send_message(message);
}

std::string ClientProtocol::receive_message() const {
    return protocol.receive_message();
}