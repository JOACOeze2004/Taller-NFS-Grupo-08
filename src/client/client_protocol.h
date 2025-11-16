#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include "../common/socket.h"
#include "../common/protocol.h"
#include "../common/DTO.h"

class ClientProtocol {
    private:
        Protocol protocol;

        CarDTO receive_car_state() const;
        LobbyCarDTO receive_lobby_car_state() const;

    public:
        explicit ClientProtocol(Socket& _socket);
        std::string receive_error_message();
        const std::vector<std::string> receive_games_list();
        void send_player_config(const std::string& name, uint8_t car_id,
                            const std::string& map_name) ;
        void receive_game_init_data(std::string& map_path,
                                   float& spawn_x, float& spawn_y) ;
        void send_lobby_action(uint8_t action, const std::string& game_id);

        void send_byte(const uint8_t byte) const;
        uint8_t receive_byte() const;
        Snapshot receive_game_state() const;
        FinalScoreList receive_final_results() const;
        void close();
};

#endif  // CLIENT_PROTOCOL_H
