#include "server_protocol.h"
#include <cstring>

ServerProtocol::ServerProtocol(Socket& _socket) : protocol(_socket) {  }

uint8_t ServerProtocol::receive_standar_command() const {
    return protocol.receive_byte();
}

void ServerProtocol::send_car_state(const CarDTO& car){
    protocol.send_float(car.x);
    protocol.send_float(car.y);
    protocol.send_float(car.velocity);
    protocol.send_float(car.angle);
}

void ServerProtocol::send_game_init_data(const std::string& map_path,
                                   float spawn_x, float spawn_y) {
    auto path_length = static_cast<uint16_t>(map_path.size());
    protocol.send_big_endian_16(path_length);
    protocol.send_string(map_path);

    uint32_t x_bytes;
    uint32_t y_bytes;
    std::memcpy(&x_bytes, &spawn_x, sizeof(float));
    std::memcpy(&y_bytes, &spawn_y, sizeof(float));

    protocol.send_big_endian_32(x_bytes);
    protocol.send_big_endian_32(y_bytes);
}

void ServerProtocol::receive_player_config(std::string& name, uint8_t& car_id,
                                     std::string& map_name) {

    uint16_t name_length = protocol.receive_big_endian_16();
    name = protocol.receive_string(name_length);

    car_id = protocol.receive_byte();
    uint16_t map_length = protocol.receive_big_endian_16();
    map_name = protocol.receive_string(map_length);
}

void ServerProtocol::receive_lobby_action(uint8_t& action, std::string& game_id) {
    action = protocol.receive_byte();
    
    if (action == SEND_JOIN_GAME) {
        uint16_t id_len = protocol.receive_big_endian_16();
        if (id_len > 0) {
            game_id = protocol.receive_string(id_len);
        }
    }
}

void ServerProtocol::send_game_state(const DTO& dto) {
    protocol.send_byte(static_cast<uint8_t>(dto.id));    
    protocol.send_big_endian_16(dto.cars.size());
    
    for (const auto& [car_id, car] : dto.cars) {
        send_car_state(car); 
    }

    // protocol.send_byte(static_cast<uint8_t>(snap.state));
    // protocol.send_big_endian_16(snap.cars_count);
    // protocol.send_byte(static_cast<uint8_t>(snap.map));
    // protocol.send_byte(static_cast<uint8_t>(snap.upgrade));
    // protocol.send_bool(snap.upgradeable);
    // protocol.send_byte(static_cast<uint8_t>(snap.collision));
    // protocol.send_bool(snap.under_bridge);
    // protocol.send_float(snap.checkpoint.x);
    // protocol.send_float(snap.checkpoint.y);
    // protocol.send_byte(static_cast<uint8_t>(snap.type_checkpoint));
}

void ServerProtocol::close(){
    protocol.close_socket();    
}