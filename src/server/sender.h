#ifndef CLIENT_SENDER_H
#define CLIENT_SENDER_H

#include <string>
#include <utility>

#include <arpa/inet.h>

#include "../common/constants.h"
#include "../common/queue.h"
#include "../common/DTO.h"
#include "../common/thread.h"

#include "car_state.h"
#include "server_protocol.h"

class ClientSender: public Thread {
    ServerProtocol& protocol;
    Queue<DTO>& client_queue;

public:
    ClientSender(ServerProtocol& protocol, Queue<DTO>& queue);
    void run() override;

    ~ClientSender();
};

#endif  // CLIENT_SENDER_H
