#include "client_protocol.h"
#include <cstring>
#include "InvalidId.h"

ClientProtocol::ClientProtocol(Socket& _socket) : protocol(_socket) {  }

CarDTO ClientProtocol::receive_car_state() const {
    CarDTO car;
    car.x = protocol.receive_float();
    car.y = protocol.receive_float();
    car.velocity = protocol.receive_float();
    car.angle = protocol.receive_float();
    car.car_id = protocol.receive_big_endian_32();

    //protocol.receive_bool(car.under_bridge); VER SI QUEDA O SE VA

    car.life = protocol.receive_float();
    car.nitro = protocol.receive_bool();
    car.remaining_nitro = protocol.receive_float();
    car.state = static_cast<StateRunning>(protocol.receive_byte());
    return car;
}

LobbyCarDTO ClientProtocol::receive_lobby_car_state() const{
    LobbyCarDTO lobby_car;
    lobby_car.acceleration = protocol.receive_float();
    lobby_car.brake = protocol.receive_float();
    lobby_car.handling = protocol.receive_float();
    lobby_car.life = protocol.receive_float();
    lobby_car.mass = protocol.receive_float();

    return lobby_car;
}


void ClientProtocol::send_byte(const uint8_t byte) const {
    protocol.send_byte(byte);
}

uint8_t ClientProtocol::receive_byte() const{
    return protocol.receive_byte();
}

const std::vector<std::string> ClientProtocol::receive_games_list(){
    std::vector<std::string> games;
    uint16_t game_amount = protocol.receive_big_endian_16();
    for (int i = 0; i < game_amount; i++){
        uint16_t game_len = protocol.receive_big_endian_16();
        std::string g = protocol.receive_string(game_len);
        games.push_back(g);   
    }
    return games;
}

void ClientProtocol::send_lobby_action(uint8_t action, const std::string& game_id) {
    protocol.send_byte(action);
    if (action == SEND_JOIN_GAME) {
        uint16_t id_len = static_cast<uint16_t>(game_id.size());
        protocol.send_big_endian_16(id_len);
        if (id_len > 0) {
            protocol.send_string(game_id);
        }
    }
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

Snapshot ClientProtocol::receive_game_state() const {
    Snapshot snapshot;
    snapshot.id = protocol.receive_byte();
    if (snapshot.id == 0) {
        throw InvalidId();
    }
    uint16_t cars_count = protocol.receive_big_endian_16();

    for (auto i = 0; i < cars_count; ++i) {
        int car_id = protocol.receive_big_endian_16(); 
        CarDTO car = this->receive_car_state();
        snapshot.cars[car_id] = car;
    }

    snapshot.game_id = protocol.receive_byte();

    snapshot.position = protocol.receive_byte();
    
    snapshot.cars_count = cars_count;

    snapshot.map = static_cast<Map>(protocol.receive_byte());
    snapshot.upgrade = static_cast<Upgrades>(protocol.receive_byte());
    snapshot.upgradeable = protocol.receive_bool();
    snapshot.collision = static_cast<CollisionType>(protocol.receive_byte());
    snapshot.checkpoint.x = protocol.receive_float();
    snapshot.checkpoint.y = protocol.receive_float();
    snapshot.hint.x = protocol.receive_float();
    snapshot.hint.y = protocol.receive_float();
    snapshot.hint.angle = protocol.receive_float();
    snapshot.type_checkpoint = static_cast<TypeCheckpoint>(protocol.receive_byte());
    snapshot.time_ms = protocol.receive_big_endian_32();
    
    uint16_t lobby_count = protocol.receive_big_endian_16();

    for (size_t i = 0; i < lobby_count; i++){
        int car_id = protocol.receive_big_endian_16(); 
        LobbyCarDTO lobby_car = this->receive_lobby_car_state();
        snapshot.lobby_cars[car_id] = lobby_car;
    }
    return snapshot;
}

std::string ClientProtocol::receive_error_message(){
    size_t size = protocol.receive_big_endian_16();
    return protocol.receive_string(size);
}

void ClientProtocol::close(){
    protocol.close_socket();
}
