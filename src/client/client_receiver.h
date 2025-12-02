#ifndef TALLER_TP_CLIENT_RECEIVER_H
#define TALLER_TP_CLIENT_RECEIVER_H

#include "../common/thread.h"
#include "../common/queue.h"

#include "client_protocol.h"
#include "../common/DTO.h"


class ClientReceiver : public Thread  {
    ClientProtocol& protocol;
    Queue<Snapshot> queue;
    FinalScoreList final_results;
    bool has_finals;
    bool server_disconnected = false;

public:
    explicit ClientReceiver(ClientProtocol& protocol);
    void run() override;
    bool try_pop_snapshot(Snapshot& snapshot);
    bool has_final_results() const;
    bool is_server_disconnected() const;
    FinalScoreList get_final_results();
};

#endif  // TALLER_TP_CLIENT_RECEIVER_H
