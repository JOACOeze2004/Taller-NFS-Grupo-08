#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include "../common/socket.h"
#include "../common/protocol.h"
#include "login/login_window.h"

class ClientProtocol {
    private:
        Protocol protocol;
    public:
        explicit ClientProtocol(Socket& _socket);
        void send_player_config(const std::string& name, uint8_t car_id,
                            const std::string& map_name) ;
        void receive_game_init_data(std::string& map_path,
                                   float& spawn_x, float& spawn_y) ;

        CarDTO receive_car_state() const;
        void send_byte(const uint8_t byte) const;
        uint8_t receive_byte() const;
        void close();
};

#endif  // CLIENT_PROTOCOL_H
