#include "client_protocol.h"
#include <cstring>
#include "InvalidId.h"

ClientProtocol::ClientProtocol(Socket& _socket) : protocol(_socket) {  }

std::unordered_map<int, CarDTO> ClientProtocol::receive_cars(const uint16_t cars_count) const{
    std::unordered_map<int, CarDTO> cars;
   
    for (auto i = 0; i < cars_count; ++i) {
        int car_id = protocol.receive_big_endian_16(); 
        cars[car_id] = this->receive_car_state();
    }
    return cars;
}

void ClientProtocol::receive_game_data(Snapshot& snapshot, const uint16_t cars_count) const {
    snapshot.game_id = protocol.receive_byte();
    snapshot.position = protocol.receive_byte();
    snapshot.cars_count = cars_count;
    snapshot.map = static_cast<Map>(protocol.receive_byte());
    snapshot.upgrade = static_cast<Upgrades>(protocol.receive_byte());
    snapshot.upgradeable = protocol.receive_bool();
    snapshot.collision = static_cast<CollisionType>(protocol.receive_byte());
}

void ClientProtocol::receive_checkpoint_hint(Snapshot& snapshot) const {
    snapshot.checkpoint.x = protocol.receive_float();
    snapshot.checkpoint.y = protocol.receive_float();
    snapshot.hint.x = protocol.receive_float();
    snapshot.hint.y = protocol.receive_float();
    snapshot.hint.angle = protocol.receive_float();
    snapshot.type_checkpoint = static_cast<TypeCheckpoint>(protocol.receive_byte());
    snapshot.total_checkpoints = protocol.receive_big_endian_16();
    snapshot.current_checkpoint = protocol.receive_big_endian_16();
}

void ClientProtocol::receive_remaining_data(Snapshot& snapshot) const {
    snapshot.time_ms = protocol.receive_big_endian_32();
    snapshot.state = static_cast<State>(protocol.receive_byte());
    snapshot.is_owner = protocol.receive_bool();
}

void ClientProtocol::receive_lobby_cars(Snapshot& snapshot) const {
    uint16_t lobby_count = protocol.receive_big_endian_16();
    for (size_t i = 0; i < lobby_count; i++){
        int car_id = protocol.receive_big_endian_16(); 
        LobbyCarDTO lobby_car = this->receive_lobby_car_state();
        snapshot.lobby_cars[car_id] = lobby_car;
    }
}

void ClientProtocol::receive_prices(Snapshot& snapshot) const {
    uint16_t prices_count = protocol.receive_big_endian_16();
    for (int i = 0; i < prices_count; i++) {
        Upgrades upgrade = static_cast<Upgrades>(protocol.receive_byte());
        int seconds = protocol.receive_big_endian_16();
        snapshot.prices[upgrade] = std::chrono::seconds(seconds);
    }
}

std::vector<CarRacingInfo> ClientProtocol::receive_cars_finished() const {
    uint16_t cars_count = protocol.receive_big_endian_16();
    std::vector<CarRacingInfo> cars;
    cars.reserve(cars_count);
    
    for (uint16_t i = 0; i < cars_count; i++) {
        CarRacingInfo car;
        size_t name_size = protocol.receive_big_endian_16();
        car.name = protocol.receive_string(name_size);
        car.time = protocol.receive_big_endian_32();
        car.position = protocol.receive_big_endian_32();
        cars.push_back(car);
    }
    
    return cars;
}

void ClientProtocol::receive_player_total_times(Snapshot& snapshot) const {
    uint16_t total_times_count = protocol.receive_big_endian_16();
    
    for (uint16_t i = 0; i < total_times_count; i++) {
        int player_id = protocol.receive_big_endian_16();
        long long total_time = protocol.receive_big_endian_64();
        snapshot.player_total_times[player_id] = total_time;
    }
}

void ClientProtocol::receive_car_upgrades(Snapshot& snapshot) const {
    uint16_t upgrades_count = protocol.receive_big_endian_16();
    for (int i = 0; i < upgrades_count; i++) {
        Upgrades type = static_cast<Upgrades>(protocol.receive_byte());
        int level = protocol.receive_big_endian_16();
        snapshot.upgrades[type] = level;
    }
}

CarDTO ClientProtocol::receive_car_state() const {
    CarDTO car;
    car.x = protocol.receive_float();
    car.y = protocol.receive_float();
    car.velocity = protocol.receive_float();
    car.angle = protocol.receive_float();
    car.car_id = protocol.receive_big_endian_16();

    car.life = protocol.receive_float();
    car.nitro = protocol.receive_bool();
    car.remaining_nitro = protocol.receive_float();
    car.state = static_cast<StateRunning>(protocol.receive_byte());
    car.remaining_upgrades = protocol.receive_big_endian_16();
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


void ClientProtocol::send_byte(const uint8_t byte) const { protocol.send_byte(byte); }

uint8_t ClientProtocol::receive_byte() const { return protocol.receive_byte(); }

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

void ClientProtocol::send_player_config(const std::string& name, uint8_t car_id, const std::string& map_name) {
    auto name_length = static_cast<uint16_t>(name.size());
    protocol.send_big_endian_16(name_length);
    protocol.send_string(name);
    send_byte(car_id);
    auto map_length = static_cast<uint16_t>(map_name.size());
    protocol.send_big_endian_16(map_length);
    protocol.send_string(map_name);
}

CarRacingInfo ClientProtocol::receive_single_racing_info() const {
    CarRacingInfo result;
    uint16_t name_len = protocol.receive_big_endian_16();
    result.name = protocol.receive_string(name_len);
    result.time = protocol.receive_float();
    result.position = static_cast<int>(protocol.receive_byte());
    
    return result;
}

void ClientProtocol::receive_game_init_data(std::string& map_path, float& spawn_x, float& spawn_y) {
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
    uint16_t cars_count = protocol.receive_big_endian_16();
    snapshot.cars = this->receive_cars(cars_count);

    receive_game_data(snapshot, cars_count);
    receive_checkpoint_hint(snapshot);
    receive_remaining_data(snapshot);
    receive_lobby_cars(snapshot);
    receive_prices(snapshot);
    
    snapshot.cars_finished = receive_cars_finished();
    receive_player_total_times(snapshot);
    receive_car_upgrades(snapshot);

    return snapshot;
}

std::string ClientProtocol::receive_error_message(){
    size_t size = protocol.receive_big_endian_16();
    return protocol.receive_string(size);
}

FinalScoreList ClientProtocol::receive_final_results() const {
    uint16_t count = protocol.receive_big_endian_16();
    FinalScoreList results;
    results.reserve(count);

    for (uint16_t i = 0; i < count; ++i) {
        results.push_back(receive_single_racing_info());
    }

    return results;
}

void ClientProtocol::close(){
    protocol.close_socket();
}
