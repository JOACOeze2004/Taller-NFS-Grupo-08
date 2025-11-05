#include "sender.h"

ClientSender::ClientSender(ServerProtocol& prot, Queue<DTO>& queue):
        protocol(prot), client_queue(queue) {}

void ClientSender::run() {
    while (should_keep_running()) {
        try {
            DTO dto = client_queue.pop();
            if (dto.id != 0){
                protocol.send_game_state(dto);
            }           
        } catch (const ClosedQueue&) {
            this->stop();
            break;
        }
    }
}

ClientSender::~ClientSender() {}
