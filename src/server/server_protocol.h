#ifndef TALLER_TP_SERVER_PROTOCOL_H
#define TALLER_TP_SERVER_PROTOCOL_H

#include "../common/protocol.h"
#include "src/common/car_DTO.h"

#include "car_state.h"


class ServerProtocol {
private:
    Protocol protocol;
public:
    explicit ServerProtocol(Socket& socket);

    void receive_player_config(std::string& name, uint8_t& car_id,
                              std::string& map_name) ;
    void send_game_init_data(const std::string& map_path,
                            float spawn_x, float spawn_y) ;
    uint8_t receive_standar_command() const; //a prioiri para el movimiento
    void send_car_state(CarDTO& car);

    void close();

};

#endif  // TALLER_TP_SERVER_PROTOCOL_H
