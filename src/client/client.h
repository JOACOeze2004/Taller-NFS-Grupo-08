#ifndef TALLER_TP_CLIENT_H
#define TALLER_TP_CLIENT_H

#include <iostream>
#include <string>

#include "../common/socket.h"
#include "login/login_window.h"

#include "client_protocol.h"

class Client {
public:
    Client(const std::string& host, const std::string& port);
    void run();
    void send_config(const PlayerConfig& config,uint8_t lobby_action, const std::string& game_id);

private:
    std::string host;
    std::string port;
    Socket client_socket;
    ClientProtocol protocol;
};

#endif  // TALLER_TP_CLIENT_H
