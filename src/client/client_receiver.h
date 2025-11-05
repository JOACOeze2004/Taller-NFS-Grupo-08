#ifndef TALLER_TP_CLIENT_RECEIVER_H
#define TALLER_TP_CLIENT_RECEIVER_H

#include "../common/thread.h"
#include "../common/queue.h"

#include "client_protocol.h"
#include "../common/DTO.h"


class ClientReceiver : public Thread  {
    ClientProtocol& protocol;
    Queue<DTO> queue;

public:
    explicit ClientReceiver(ClientProtocol& protocol); // seguro tendrá el parser que actualiza el juego
    void run() override;
    bool try_pop_car_state(DTO& dto); // Ver si nos sirve en un futuro o lo sacamos (lo vi en el tp del CS)

};

#endif  // TALLER_TP_CLIENT_RECEIVER_H
