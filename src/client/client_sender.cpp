#include "client_sender.h"
#include "../common/constants.h"

ClientSender::ClientSender(ClientProtocol& _protocol, Queue<Command>& _command_queue)
    : protocol(_protocol), command_queue(_command_queue) {}

void ClientSender::run() {
    try {
        while (should_keep_running()) {
            Command cmd = command_queue.pop();
            
            protocol.send_byte(cmd.cmd);

        }
    } catch (const std::exception& e) {
        this->stop();
    }
}

ClientSender::~ClientSender() {}