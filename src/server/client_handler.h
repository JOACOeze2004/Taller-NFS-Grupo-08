#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <string>
#include <utility>

#include <arpa/inet.h>

#include "../common/car_DTO.h"
#include "../common/queue.h"
#include "../common/socket.h"
#include "../common/thread.h"
#include "src/common/DTO.h"

#include "car_state.h"
#include "client_command.h"
#include "sender.h"
#include "server_protocol.h"

class Monitor;
class Gameloop;
class ClientReceiver;

class ClientHandler: public Thread {
private:
    Socket peer; 
    ServerProtocol protocol;
    Queue<Snapshot  > client_queue;
    Monitor& monitor;
    std::string game_id;
    int id;
    std::shared_ptr<ClientReceiver> receiver;
    ClientSender sender;

    std::string player_name;
    uint8_t car_id{};
    std::string map_name;
public:
    explicit ClientHandler(Socket&& peer,Monitor& monitor, int _id);
    void run() override;
    void send_state(Snapshot snapshot);
    void kill();
    void kill_threads();
    bool is_dead() const;
    void set_game_id(const std::string& _game_id);
    const std::string& get_game_id() const;
    void send_final_results(const FinalScoreList& results);
    ~ClientHandler();
};

#endif  // CLIENT_HANDLER_H
