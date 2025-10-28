#ifndef TALLER_TP_CLIENT_SENDER_H
#define TALLER_TP_CLIENT_SENDER_H

#include "client_protocol.h"
#include "../common/queue.h"
#include "../common/command.h"
#include "../common/thread.h"

class ClientSender : public Thread {
    ClientProtocol& protocol;
    Queue<Command>& command_queue;

    public:
    explicit ClientSender(ClientProtocol& _protocol, Queue<Command>& _command_queue);
    void run() override;

    ~ClientSender();
};


#endif  // TALLER_TP_CLIENT_SENDER_H


