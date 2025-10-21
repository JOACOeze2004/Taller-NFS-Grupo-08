#ifndef TALLER_TP_SERVER_PROTOCOL_H
#define TALLER_TP_SERVER_PROTOCOL_H

#include "../common/protocol.h"

class ServerProtocol {
private:
    Protocol protocol;
public:
    explicit ServerProtocol(Socket& socket);
    void send_message(const std::string& message) const;
    std::string receive_message() const;
    void close();

};

#endif  // TALLER_TP_SERVER_PROTOCOL_H
