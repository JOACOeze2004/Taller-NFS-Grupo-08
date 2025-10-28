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
#include "client_command.h"


class ClientReceiver: public Thread {
    ServerProtocol& protocol;
    Queue<ClientCommand>& command_queue;
    int reciver_id;

public:
    ClientReceiver(ServerProtocol& prot, Queue<ClientCommand>& queue, int id);
    void run() override;

    ~ClientReceiver();
};

#endif  // CLIENT_Receiver_H
