#ifndef TALLER_TP_SERVER_PROTOCOL_H
#define TALLER_TP_SERVER_PROTOCOL_H

#include "../common/protocol.h"
#include "src/common/car_DTO.h"

#include "car_state.h"
#include "../common/DTO.h"

class ServerProtocol {
private:
    Protocol protocol;
public:
    explicit ServerProtocol(Socket& socket);

    void send_games_list(const std::vector<std::string>& games);

    void receive_player_config(std::string& name, uint8_t& car_id,
                              std::string& map_name) ;
    void send_game_init_data(const std::string& map_path,
                            float spawn_x, float spawn_y) ;

    void receive_lobby_action(uint8_t& action, std::string& game_id);
    uint8_t receive_standar_command() const; //a prioiri para el movimiento
    void send_car_state(const CarDTO& car);

    void send_game_state(const DTO& dto);

    void close();

};

#endif  // TALLER_TP_SERVER_PROTOCOL_H
