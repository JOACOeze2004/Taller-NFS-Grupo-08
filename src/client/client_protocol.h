#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include "../common/socket.h"
#include "src/common/protocol.h"

class ClientProtocol {
    private:
        Protocol protocol;
    public:
        explicit ClientProtocol(Socket& _socket);
        void send_message(const std::string& message) const;
        std::string receive_message() const;

};

#endif  // CLIENT_PROTOCOL_H
