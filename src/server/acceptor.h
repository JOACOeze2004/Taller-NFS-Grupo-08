#ifndef CLIENT_ACCEPTOR_H
#define CLIENT_ACCEPTOR_H

#include <string>
#include <vector>

#include "../common/socket.h"
#include "../common/thread.h"

#include "client_handler.h"
#include "monitor.h"

class ClientAcceptor: public Thread {
    Socket acceptor;
    std::vector<ClientHandler*> clients;
    Queue<std::string> command_queue;
    Monitor& monitor;
    int next_id;

public:
    explicit ClientAcceptor(const std::string& port, Monitor& _monitor);
    void run() override;
    void clear();
    void reap();
    void close_acceptor_socket();
    ~ClientAcceptor();
};

#endif  // CLIENT_ACCEPTOR_H
