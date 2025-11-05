#include "server.h"

#include <cstdint>
#include <iostream>

#include "../common/constants.h"

#include "acceptor.h"
#include "monitor.h"

Server::Server(const std::string& port): port(port){}

void Server::run() {
    Monitor monitor;
    ClientAcceptor acceptor(port, monitor);
    std::cout << "[SERVER] Listening on port " << port << std::endl;

    std::cout << "[SERVER] Acceptor started " << std::endl;
    acceptor.start();
    std::string line;
    while (std::getline(std::cin, line)) {
        if (!line.empty() && line[0] == SERVER_CLOSE) {
            break;
        }
    }
    std::cout << "[SERVER] Acceptor close" << std::endl;
    acceptor.close_acceptor_socket();
    acceptor.join();
    std::cout << "[SERVER] Connection closed" << std::endl;
}