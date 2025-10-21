#ifndef CLIENT_SENDER_H
#define CLIENT_SENDER_H

#include <string>
#include <utility>

#include <arpa/inet.h>

#include "../common/constants.h"
#include "../common/queue.h"
#include "../common/socket.h"
#include "../common/thread.h"

#include "server_protocol.h"

class ClientSender: public Thread {
    ServerProtocol& protocol;
    Queue<std::string>& client_queue;

public:
    ClientSender(ServerProtocol& protocol, Queue<std::string>& queue);
    void run() override;

    ~ClientSender();
};

#endif  // CLIENT_SENDER_H
