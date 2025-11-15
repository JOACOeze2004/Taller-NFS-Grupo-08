#include "client_receiver.h"

#include "InvalidId.h"

ClientReceiver::ClientReceiver(ClientProtocol& protocol) : protocol(protocol) {}

void ClientReceiver::run(){
    while (this->should_keep_running()) {
        try {
            Snapshot snapshot = protocol.receive_game_state();
            queue.push(snapshot);
        }
        catch(const std::exception& e) {
            this->stop();
            break;
        } 
    }    
}

bool ClientReceiver::try_pop_car_state(Snapshot& snapshot) {
    return queue.try_pop(snapshot);
}
