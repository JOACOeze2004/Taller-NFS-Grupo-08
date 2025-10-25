#include "sender.h"

ClientSender::ClientSender(ServerProtocol& prot, Queue<CarDTO>& queue):
        protocol(prot), client_queue(queue) {}

void ClientSender::run() {
    while (should_keep_running()) {
        try {
            CarDTO carState = client_queue.pop();
            protocol.send_car_state(carState);
        } catch (const ClosedQueue&) {
            this->stop();
            break;
        }
    }
}

ClientSender::~ClientSender() {}
