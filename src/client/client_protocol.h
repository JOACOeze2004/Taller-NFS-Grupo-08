#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include "../common/socket.h"
#include "../common/protocol.h"

class ClientProtocol {
    private:
        Protocol protocol;
    public:
        explicit ClientProtocol(Socket& _socket);
        void send_message(const std::string& message) const;
        std::string receive_message() const;

        CarState receive_car_state() const;
        void send_byte(const uint8_t byte) const;
        uint8_t receive_byte() const;
        void close();
};

#endif  // CLIENT_PROTOCOL_H
