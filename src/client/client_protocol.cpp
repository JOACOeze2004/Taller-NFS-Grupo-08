#include "client_protocol.h"
#include <cstring>

ClientProtocol::ClientProtocol(Socket& _socket) : protocol(_socket) {  }


void ClientProtocol::send_message(const std::string& message) const {
    protocol.send_message(message);
}

std::string ClientProtocol::receive_message() const {
    return protocol.receive_message();
}

CarDTO ClientProtocol::receive_car_state() const {
    return protocol.receive_car_state();
}

void ClientProtocol::send_byte(const uint8_t byte) const {
    protocol.send_byte(byte);
}

uint8_t ClientProtocol::receive_byte() const{
    return protocol.receive_byte();
}

void ClientProtocol::close(){
    protocol.close_socket();
}

void ClientProtocol::send_player_config(const std::string& name, uint8_t car_id,
                                  const std::string& map_name) {
    auto name_length = static_cast<uint16_t>(name.size());
    protocol.send_big_endian_16(name_length);
    protocol.send_string(name);
    send_byte(car_id);

    auto map_length = static_cast<uint16_t>(map_name.size());
    protocol.send_big_endian_16(map_length);
    protocol.send_string(map_name);
}

void ClientProtocol::receive_game_init_data(std::string& map_path,
                                      float& spawn_x, float& spawn_y) {
    uint16_t path_length = protocol.receive_big_endian_16();
    map_path = protocol.receive_string(path_length);

    uint32_t x_bytes = protocol.receive_big_endian_32();
    uint32_t y_bytes = protocol.receive_big_endian_32();

    std::memcpy(&spawn_x, &x_bytes, sizeof(float));
    std::memcpy(&spawn_y, &y_bytes, sizeof(float));
}