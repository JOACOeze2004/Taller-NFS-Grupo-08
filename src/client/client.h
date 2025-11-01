#ifndef TALLER_TP_CLIENT_H
#define TALLER_TP_CLIENT_H

#include "../common/socket.h"
#include <string>
#include <iostream>
#include "login/login_window.h"

class Client {
public:
    Client(const std::string& host, const std::string& port);
    void run(const PlayerConfig& config,uint8_t lobby_action, const std::string& game_id);

private:
    std::string host;
    std::string port;
    Socket client_socket;
};

#endif  // TALLER_TP_CLIENT_H
