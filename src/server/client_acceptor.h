#ifndef CLIENT_ACCEPTOR_H
#define CLIENT_ACCEPTOR_H

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include <arpa/inet.h>

#include "../common/constants.h"
#include "../common/liberror.h"
#include "../common/socket.h"
#include "../common/thread.h"

#include "client_handler.h"


class ClientAcceptor: public Thread {
private:
    Socket acceptor;
    std::vector<ClientHandler*> clients;
    Queue<std::string> command_queue;

public:
    explicit ClientAcceptor(const std::string& port);
    void run() override;
    void clear();
    void reap();
    void close_acceptor_socket();
    ~ClientAcceptor();
};

#endif  // CLIENT_ACCEPTOR_H
