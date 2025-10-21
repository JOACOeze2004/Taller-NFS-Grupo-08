#ifndef TALLER_TP_SERVER_H
#define TALLER_TP_SERVER_H

#include <string>

#include "../common/socket.h"

#include "acceptor.h"

class Server {
public:
    explicit Server(const std::string& port);
    void run();

private:
    std::string port;
    //Socket server_socket;
};

#endif  // TALLER_TP_SERVER_H
