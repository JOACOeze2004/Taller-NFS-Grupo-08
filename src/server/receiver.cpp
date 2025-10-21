#include "receiver.h"

ClientReceiver::ClientReceiver(ServerProtocol& prot, Queue<std::string>& queue):
        protocol(prot), command_queue(queue) {}

void ClientReceiver::run() {
    std::cout << "[Receiver] reciver loop " << std::endl;
    while (should_keep_running()) {
        try {
            std::string request = protocol.receive_message();
            command_queue.push(request);
        } catch (const std::exception& e) {
            this->stop();
            break;
        }
    }
}

ClientReceiver::~ClientReceiver() {}
