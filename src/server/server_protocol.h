#ifndef TALLER_TP_SERVER_PROTOCOL_H
#define TALLER_TP_SERVER_PROTOCOL_H

#include "../common/protocol.h"
#include "car_state.h"


class ServerProtocol {
private:
    Protocol protocol;
public:
    explicit ServerProtocol(Socket& socket);
    void send_message(const std::string& message) const;

    
    uint8_t receive_standar_command() const; //a prioiri para el movimiento
    std::string receive_message() const;
    CarState receive_car_state();
    void send_car_state(CarState car);

    void close();

};

#endif  // TALLER_TP_SERVER_PROTOCOL_H
