#include "sender.h"

ClientSender::ClientSender(ServerProtocol& prot, Queue<std::string>& queue):
        protocol(prot), client_queue(queue) {}

void ClientSender::run() {
    std::cout << "[Sender] sender loop " << std::endl;
    while (should_keep_running()) {
        try {
            std::string msg = client_queue.pop();
            protocol.send_message(msg);
        } catch (const ClosedQueue&) {
            break;
        }
    }
}

ClientSender::~ClientSender() {}
