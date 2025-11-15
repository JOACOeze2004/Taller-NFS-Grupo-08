#include "server_protocol.h"
#include <cstring>

ServerProtocol::ServerProtocol(Socket& _socket) : protocol(_socket) {  }

uint8_t ServerProtocol::receive_standar_command() const {
    return protocol.receive_byte();
}

void ServerProtocol::send_cars(const Snapshot& snapshot){
    protocol.send_big_endian_16(snapshot.cars.size());
    for (const auto& [car_id, car] : snapshot.cars) {
        protocol.send_big_endian_16(car_id);
        send_car_state(car); 
    }
}

void ServerProtocol::send_game_data(const Snapshot& snapshot){
    protocol.send_byte(static_cast<uint8_t>(snapshot.game_id));
    protocol.send_byte(static_cast<uint8_t>(snapshot.position));
    protocol.send_byte(static_cast<uint8_t>(snapshot.map));
    protocol.send_byte(static_cast<uint8_t>(snapshot.upgrade));
    protocol.send_bool(snapshot.upgradeable);
    protocol.send_byte(static_cast<uint8_t>(snapshot.collision));
}

void ServerProtocol::send_checkpoint_hint(const Snapshot& snapshot){
    protocol.send_float(snapshot.checkpoint.x);
    protocol.send_float(snapshot.checkpoint.y);
    protocol.send_float(snapshot.hint.x);
    protocol.send_float(snapshot.hint.y);
    protocol.send_float(snapshot.hint.angle);
    protocol.send_byte(static_cast<uint8_t>(snapshot.type_checkpoint));
}

void ServerProtocol::send_remaining_data(const Snapshot& snapshot){
    protocol.send_big_endian_32(snapshot.time_ms);
    protocol.send_byte(static_cast<uint8_t>(snapshot.state));
    protocol.send_bool(snapshot.is_owner);
}

void ServerProtocol::send_lobby_cars(const Snapshot& snapshot){
    protocol.send_big_endian_16(snapshot.lobby_cars.size());
    for (const auto& [car_id, lobby_car] : snapshot.lobby_cars) {
        send_lobby_car_state(lobby_car); 
    }
}

void ServerProtocol::send_prices(const Snapshot& snapshot){
    protocol.send_big_endian_16(snapshot.prices.size());
    for (const auto& [upgrade, secs] : snapshot.prices) {
        protocol.send_byte(static_cast<uint8_t>(upgrade));
        protocol.send_big_endian_16(secs.count());
    }
}

void ServerProtocol::send_car_state(const CarDTO& car){
    protocol.send_float(car.x);
    protocol.send_float(car.y);
    protocol.send_float(car.velocity);
    protocol.send_float(car.angle);
    protocol.send_big_endian_32(car.car_id);
    
    float life_percentage = (static_cast<float>(car.life) * 100.0f) / static_cast<float>(MAX_LIFE);

    protocol.send_float(life_percentage);
    protocol.send_bool(car.nitro);
    protocol.send_float(car.remaining_nitro);
    protocol.send_byte(static_cast<uint8_t>(car.state));
}

void ServerProtocol::send_lobby_car_state(const LobbyCarDTO& car) {
    protocol.send_big_endian_16(car.car_id);
    protocol.send_float(car.acceleration);
    protocol.send_float(car.brake);
    protocol.send_float(car.handling);
    protocol.send_float(car.life);
    protocol.send_float(car.mass);
}

void ServerProtocol::send_games_list(const std::vector<std::string>& games) {
    protocol.send_big_endian_16(games.size());
    for (const auto& g : games) {
        protocol.send_big_endian_16(g.size());
        protocol.send_string(g);
    }
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

void ServerProtocol::send_game_state(const Snapshot& snapshot) {
    protocol.send_byte(static_cast<uint8_t>(snapshot.id));    
    send_cars(snapshot);
    send_game_data(snapshot);
    send_checkpoint_hint(snapshot);
    send_remaining_data(snapshot);
    send_lobby_cars(snapshot);
    send_prices(snapshot);
}

void ServerProtocol::send_error_message(const std::string& msg) {
    uint8_t signal = SEND_ERROR_MESSAGE;
    protocol.send_byte(signal);
    
    protocol.send_big_endian_16(static_cast<uint16_t>(msg.size()));
    protocol.send_string(msg);
}

void ServerProtocol::send_ok() {
    protocol.send_byte(SEND_OK_MESSAGE);
}

void ServerProtocol::close(){
    protocol.close_socket();    
}