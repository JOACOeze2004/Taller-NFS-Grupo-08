#include "client.h"
#include "client_protocol.h"

Client::Client(const std::string& host, const std::string& port)
    : host(host), port(port), client_socket(host.c_str(), port.c_str()) {}

void Client::run() {
    std::cout << "[CLIENT] Connected to " << host << ":" << port << std::endl;

    const ClientProtocol protocol(client_socket);
    protocol.send_message("Hello from client");

    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }

    client_socket.shutdown(2);
    client_socket.close();
}