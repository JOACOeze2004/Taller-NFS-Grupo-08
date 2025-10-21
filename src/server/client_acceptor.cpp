#include "client_acceptor.h"

ClientAcceptor::ClientAcceptor(const std::string& port, Monitor& _monitor): acceptor(port.c_str()), monitor(_monitor), next_id(1) {}

void ClientAcceptor::run() {
    std::cout << "[Acceptor] acceptor thread is running" << std::endl;
    while (should_keep_running()) {
        try {
            Socket peer = acceptor.accept();

            auto client =
                    std::make_unique<ClientHandler>(std::move(peer), command_queue, next_id);

            std::cout << "[Acceptor] new client added to the handler list" << std::endl;

            monitor.reap();
            
            std::cout << "[Acceptor] Handler thread is running" << std::endl;
            
            client->start();
            monitor.add_client(std::move(client));
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
