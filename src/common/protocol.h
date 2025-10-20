#ifndef TALLER_TP_PROTOCOL_H
#define TALLER_TP_PROTOCOL_H

#include <string>

#include "socket.h"



class Protocol {
public:
    explicit Protocol(Socket& socket);
    void send_message(const std::string& message) const;
    std::string receive_message() const;
    void close_socket();
private:
    Socket& socket;
};

#endif  // TALLER_TP_PROTOCOL_H
