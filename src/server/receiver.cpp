#include "receiver.h"

ClientReceiver::ClientReceiver(ServerProtocol& prot, Queue<ClientCommand>& queue, int id):
        protocol(prot), command_queue(queue), reciver_id(id) {}

void ClientReceiver::run() {
    while (should_keep_running()) {
        try {
            uint8_t request = protocol.receive_standar_command();
            if (request != 0) {
                ClientCommand client_command;
                client_command.id = this->reciver_id;
                client_command.cmd_struct.cmd = request;
                command_queue.push(client_command);
            }
            else {
                break;
            }
        } catch (const std::exception& e) {
            this->stop();
            break;
        }
    }
}

ClientReceiver::~ClientReceiver() {}
