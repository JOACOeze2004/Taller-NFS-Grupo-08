#include "client_receiver.h"

#include "InvalidId.h"

ClientReceiver::ClientReceiver(ClientProtocol& protocol) : protocol(protocol), has_finals(false) {}


void ClientReceiver::run(){
    while (this->should_keep_running()) {
        try {
            Snapshot snapshot = protocol.receive_game_state();
            if (snapshot.state == FINAL_RESULTS) {
                final_results = protocol.receive_final_results();
                has_finals = true;

                for (const auto& result : final_results) {
                    std::cout << "[CLIENT RECEIVER] Player: " << result.name
                              << " Position: " << result.position
                              << " Time: " << result.time << "s" << std::endl;
                }

                stop();
                break;
            }
            queue.push(snapshot);
        }
        catch(const std::exception& e) {
            this->stop();
            break;
        } 
    }    
}

bool ClientReceiver::try_pop_snapshot(Snapshot& snapshot) {
    return queue.try_pop(snapshot);
}

bool ClientReceiver::has_final_results() {
    std::lock_guard<std::mutex> lock(finals_mutex);
    return has_finals;
}

FinalScoreList ClientReceiver::get_final_results() {
    std::lock_guard<std::mutex> lock(finals_mutex);
    return final_results;
}