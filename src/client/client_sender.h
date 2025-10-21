#ifndef TALLER_TP_CLIENT_SENDER_H
#define TALLER_TP_CLIENT_SENDER_H

#include "client_protocol.h"
#include "../common/queue.h"
#include "../common/command.h"

class ClientSender  {
    Queue<Command>& command_queue;
    ClientProtocol& protocol;

    public:
    explicit ClientSender(ClientProtocol& _protocol, Queue<Command>& _command_queue);
    void run();

    ~ClientSender();
};


#endif  // TALLER_TP_CLIENT_SENDER_H


