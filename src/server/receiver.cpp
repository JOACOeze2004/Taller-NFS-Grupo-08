#include "receiver.h"

ClientReceiver::ClientReceiver(ServerProtocol& prot, Queue<ClientCommand>& queue, int id):
        protocol(prot), command_queue(queue), reciver_id(id) {}

void ClientReceiver::run() {
    std::cout << "[Receiver] reciver loop " << std::endl;
    while (should_keep_running()) {
        try {
            uint8_t request = protocol.receive_standar_command();
            if (request != 0) {
                ClientCommand cmd;
                cmd.id = this->reciver_id;
                cmd.cmd.cmd = request;
                command_queue.push(cmd);
            }
        } catch (const std::exception& e) {
            this->stop();
            break;
        }
    }
}

ClientReceiver::~ClientReceiver() {}
