#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <string>
#include <utility>

#include <arpa/inet.h>

#include "../common/car_DTO.h"
#include "../common/queue.h"
#include "../common/socket.h"
#include "../common/thread.h"

#include "car_state.h"
#include "receiver.h"
#include "sender.h"
#include "server_protocol.h"
#include "client_command.h"


class ClientHandler: public Thread {
private:
    Socket peer; 
    ServerProtocol protocol;

    Queue<ClientCommand>& command_queue;
    Queue<CarDTO> client_queue;
    
    int id;
    ClientReceiver receiver;
    ClientSender sender;

public:
    explicit ClientHandler(Socket&& peer, Queue<ClientCommand>& commands, int _id);
    void run() override;
    void send_state(CarDTO state);
    void kill();
    void kill_threads();
    bool is_dead() const;

    ~ClientHandler();
};

#endif  // CLIENT_HANDLER_H
