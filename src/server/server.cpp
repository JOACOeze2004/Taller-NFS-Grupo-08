#include "server.h"

#include <cstdint>
#include <iostream>

#include "../common/constants.h"

#include "acceptor.h"
#include "monitor.h"

Server::Server(const std::string& port): port(port){}

void Server::run() {
    Socket server_socket(port.c_str());
    std::cout << "[SERVER] Listening on port " << port << std::endl;

    Socket client_socket = server_socket.accept();
    std::cout << "[SERVER] Client connected" << std::endl;

    ServerProtocol protocol(client_socket);
    std::string message = protocol.receive_message();

    std::cout << "[SERVER] Received message: " << message << std::endl;
    protocol.send_message("hello from server");

    server_socket.shutdown(SHUT_RDWR);
    server_socket.close();
    client_socket.shutdown(SHUT_RDWR);
    client_socket.close();
    std::cout << "[SERVER] Connection closed" << std::endl;
}