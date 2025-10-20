#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <string>
#include <utility>

#include <arpa/inet.h>

#include "../common/constants.h"
#include "../common/queue.h"
#include "../common/socket.h"
#include "../common/thread.h"

#include "client_receiver.h"
#include "client_sender.h"
#include "server_protocol.h"

class ClientHandler: public Thread {
private:
    Socket peer; 
    ServerProtocol protocol;

    Queue<std::string>& command_queue;
    Queue<std::string>& client_queue;

    ClientReceiver receiver;
    ClientSender sender;

public:
    explicit ClientHandler(Socket&& peer, Queue<std::string>& commands,
                           Queue<std::string>& messages);
    void run() override;
    void kill();
    void kill_threads();
    bool is_dead() const;
    Queue<std::string>* get_client_queue() const;

    ~ClientHandler();
};

#endif  // CLIENT_HANDLER_H
