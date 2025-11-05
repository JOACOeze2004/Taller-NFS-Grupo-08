#include "receiver.h"

ClientReceiver::ClientReceiver(ServerProtocol& prot, std::shared_ptr<Gameloop> game_loop, int id):
        protocol(prot), game_loop(game_loop), reciver_id(id) {}

void ClientReceiver::run() {
    while (should_keep_running()) {
        try {
            uint8_t request = protocol.receive_standar_command();
            ClientCommand client_command;
            client_command.id = this->reciver_id;
            client_command.cmd_struct.cmd = request;
            game_loop->push_command(client_command);
            if (request == 0) {
                break;
            }
        } catch (const std::exception& e) {
            this->stop();
            break;
        }
    }
}

ClientReceiver::~ClientReceiver() {}
