#include "monitor.h"

void Monitor::add_client(std::unique_ptr<ClientHandler> client) {
    std::lock_guard<std::mutex> lock(mutex);
    clients.push_back(std::move(client));
}

void Monitor::reap() {
    for (size_t i = 0; i < clients.size();) {
        auto& client = clients[i];
        if (!client->is_alive()) {
            client->kill();
            client->join();
            clients.erase(clients.begin() + i);
        }else{
            i++;
        }
    }
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


