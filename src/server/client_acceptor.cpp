#include "client_acceptor.h"

ClientAcceptor::ClientAcceptor(const std::string& port): acceptor(port.c_str()) {}

void ClientAcceptor::run() {
    std::cout << "[Acceptor] acceptor thread is running" << std::endl;
    while (should_keep_running()) {
        try {
            Socket peer = acceptor.accept();

            auto* client_queue = new Queue<std::string>();
            ClientHandler* client =
                    new ClientHandler(std::move(peer), command_queue, *client_queue);
            reap();
            std::cout << "[Acceptor] new client added to the handler list" << std::endl;

            clients.push_back(client);
            
            std::cout << "[Acceptor] Handler thread is running" << std::endl;

            client->start();
        } catch (const std::exception& e) {
            break;
        }
    }
    clear();
}

void ClientAcceptor::reap() { 
    for (size_t i = 0; i < clients.size();) {
        ClientHandler* client = clients[i];
        if (!client->is_alive()) {
            client->kill();
            client->join();
            delete client;
            clients.erase(clients.begin() + i);
        }else{
            i++;
        }
    }   
}

void ClientAcceptor::clear() {
    for (ClientHandler* client : clients) {
        client->kill();
        client->join();
        Queue<std::string>* q = client->get_client_queue();
        delete q;
        delete client;
    }
    clients.clear();
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
