#include "server.h"

#include <iostream>
#include "server_protocol.h"

Server::Server(const std::string& port)
    : port(port), server_socket(port.c_str()) {}

void Server::run() {
    std::cout << "[SERVER] Listening on port " << port << std::endl;

    while (true) {
        Socket client_socket = server_socket.accept();
        std::cout << "[SERVER] Client connected" << std::endl;

        ServerProtocol protocol(client_socket);

        if (std::string message = protocol.receive_message(); !message.empty()) {
            std::cout << "[SERVER] Received: " << message << std::endl;
            protocol.send_message("Hello from server");
        }
        client_socket.shutdown(2);
        client_socket.close();
    }
}