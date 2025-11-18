#include "sender.h"

ClientSender::ClientSender(ServerProtocol& prot, Queue<Snapshot>& queue):
        protocol(prot), client_queue(queue) {}

void ClientSender::run() {
    while (should_keep_running()) {
        try {
            Snapshot snapshot = client_queue.pop();
            protocol.send_game_state(snapshot);         
        } catch (const ClosedQueue&) {
            this->stop();
            break;
        }
    }
}

ClientSender::~ClientSender() {}
