#include "sender.h"

ClientSender::ClientSender(ServerProtocol& prot, Queue<CarState>& queue):
        protocol(prot), client_queue(queue) {}

void ClientSender::run() {
    std::cout << "[Sender] sender loop " << std::endl;
    while (should_keep_running()) {
        try {
            //CarState carState = client_queue.pop();
            //protocol.send_message(carState);
        } catch (const ClosedQueue&) {
            break;
        }
    }
}

ClientSender::~ClientSender() {}
