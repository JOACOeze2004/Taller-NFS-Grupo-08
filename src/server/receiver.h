#ifndef CLIENT_Receiver_H
#define CLIENT_Receiver_H

#include <string>
#include <utility>

#include <arpa/inet.h>

#include "../common/constants.h"
#include "../common/queue.h"
#include "../common/socket.h"
#include "../common/thread.h"

#include "server_protocol.h"

class ClientReceiver: public Thread {
    ServerProtocol& protocol;
    Queue<std::string>& command_queue;

public:
    ClientReceiver(ServerProtocol& prot, Queue<std::string>& queue);
    void run() override;

    ~ClientReceiver();
};

#endif  // CLIENT_Receiver_H
