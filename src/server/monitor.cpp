#include "monitor.h"

void Monitor::add_client(std::unique_ptr<ClientHandler> client) {
    std::lock_guard<std::mutex> lock(mutex);
    clients.push_back(std::move(client));
}

void Monitor::clear_clients() {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& client : clients) {
        client->kill();
        client->join();
    }
    clients.clear();
}

void broadcast() {
    return;
}


