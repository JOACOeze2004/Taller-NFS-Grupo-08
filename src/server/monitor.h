#ifndef TALLER_TP_MONITOR_H
#define TALLER_TP_MONITOR_H

#include "client_handler.h"
#include "monitor.h"


class Monitor {
private:
    std::mutex mutex;
    std::vector<std::unique_ptr<ClientHandler>> clients;

public:
    void add_client(std::unique_ptr<ClientHandler> client);
    void broadcast();
    void clear_clients();
    void reap();
};


#endif  // TALLER_TP_MONITOR_H
