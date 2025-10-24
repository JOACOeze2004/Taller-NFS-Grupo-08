#ifndef CLIENT_SENDER_H
#define CLIENT_SENDER_H

#include <string>
#include <utility>

#include <arpa/inet.h>

#include "../common/constants.h"
#include "../common/queue.h"
#include "../common/car_DTO.h"
#include "../common/thread.h"

#include "car_state.h"
#include "server_protocol.h"

class ClientSender: public Thread {
    ServerProtocol& protocol;
    Queue<CarDTO>& client_queue;

public:
    ClientSender(ServerProtocol& protocol, Queue<CarDTO>& queue);
    void run() override;

    ~ClientSender();
};

#endif  // CLIENT_SENDER_H
