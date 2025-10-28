#include "client_receiver.h"

ClientReceiver::ClientReceiver(ClientProtocol& protocol) : protocol(protocol) {}

void ClientReceiver::run(){
    while (this->should_keep_running()) {
        try {
            CarDTO car = protocol.receive_car_state();
            queue.push(car);
        }
        catch(const std::exception& e) {
            this->stop();
            break;
        } 
    }    
}

bool ClientReceiver::try_pop_car_state(CarDTO& state) {
    return queue.try_pop(state);
}
