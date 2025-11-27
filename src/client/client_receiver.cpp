#include "client_receiver.h"

#include "InvalidId.h"

ClientReceiver::ClientReceiver(ClientProtocol& protocol) : protocol(protocol), has_finals(false), server_disconnected(false) {}


void ClientReceiver::run(){
    while (this->should_keep_running()) {
        try {
            Snapshot snapshot = protocol.receive_game_state();
            if (snapshot.state == FINAL_RESULTS) {
                final_results = protocol.receive_final_results();
                has_finals = true;
                stop();
                break;
            }
            queue.push(snapshot);
        }
        catch(const std::exception& e) {
            std::cout << e.what() << "\n";
            server_disconnected = true;
            this->stop();
            break;
        } 
    }    
}

bool ClientReceiver::try_pop_snapshot(Snapshot& snapshot) { return queue.try_pop(snapshot);}

bool ClientReceiver::is_server_disconnected() const { return server_disconnected; }

bool ClientReceiver::has_final_results() const { return has_finals; }

FinalScoreList ClientReceiver::get_final_results() { return final_results;}