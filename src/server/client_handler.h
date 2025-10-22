#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <string>
#include <utility>

#include <arpa/inet.h>

#include "../common/constants.h"
#include "../common/queue.h"
#include "../common/socket.h"
#include "../common/thread.h"

#include "car_state.h"
#include "receiver.h"
#include "sender.h"
#include "server_protocol.h"

class ClientHandler: public Thread {
private:
    Socket peer; 
    ServerProtocol protocol;

    Queue<std::string>& command_queue;
    Queue<CarState> client_queue;

    ClientReceiver receiver;
    ClientSender sender;
    int id;

public:
    explicit ClientHandler(Socket&& peer, Queue<std::string>& commands, int _id);
    void run() override;
    void send_state(CarState& state);
    void kill();
    void kill_threads();
    bool is_dead() const;

    ~ClientHandler();
};

#endif  // CLIENT_HANDLER_H
