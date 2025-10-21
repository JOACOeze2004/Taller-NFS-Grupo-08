#ifndef TALLER_TP_CLIENT_RECEIVER_H
#define TALLER_TP_CLIENT_RECEIVER_H

#include "client_protocol.h"

class ClientReceiver {
    ClientProtocol& protocol;

public:
    explicit ClientReceiver(ClientProtocol& _protocol); // seguro tendrá el parser que actualiza el juego
    void receiver_loop();
};

#endif  // TALLER_TP_CLIENT_RECEIVER_H
