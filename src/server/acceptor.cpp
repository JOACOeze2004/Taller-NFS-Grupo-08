#include "acceptor.h"

ClientAcceptor::ClientAcceptor(const std::string& port, Monitor& _monitor): acceptor(port.c_str()), monitor(_monitor), next_id(1) {}

void ClientAcceptor::run() {
    while (should_keep_running()) {
        try {
            Socket peer = acceptor.accept();
            auto client = std::make_unique<ClientHandler>(std::move(peer),this->monitor, next_id);
            monitor.reap();            
            client->start();
            monitor.add_client(next_id, std::move(client));
            next_id++;
        } catch (const std::exception& e) {
            break;
        }
    }
    clear();
}

void ClientAcceptor::reap() {     
    monitor.reap();  
}

void ClientAcceptor::clear() {
    monitor.kill_games();
    monitor.clear_clients();
}

void ClientAcceptor::close_acceptor_socket(){
    try {
        acceptor.shutdown(SHUT_RDWR);
        acceptor.close();
    } catch (const std::exception& e) {
        std::cerr << "Socket already closed in Acceptor: " << e.what() << '\n';
    }
}

ClientAcceptor::~ClientAcceptor() { }
