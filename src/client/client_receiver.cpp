#include "client_receiver.h"

ClientReceiver::ClientReceiver(ClientProtocol& protocol) : protocol(protocol) {}

void ClientReceiver::run(){
    while (this->should_keep_running()) {
        try {
            DTO dto = protocol.receive_game_state();
            queue.push(dto);
        }
        catch(const std::exception& e) {
            this->stop();
            break;
        } 
    }    
}

bool ClientReceiver::try_pop_car_state(DTO& dto) {
    return queue.try_pop(dto);
}
