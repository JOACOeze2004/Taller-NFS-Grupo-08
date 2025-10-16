#ifndef TALLER_TP_CLIENT_H
#define TALLER_TP_CLIENT_H

#include "../common/socket.h"
#include <string>

class Client {
public:
    Client(const std::string& host, const std::string& port);
    void run();

private:
    std::string host;
    std::string port;
    Socket client_socket;
};

#endif  // TALLER_TP_CLIENT_H
