#include "gameloop.h"

#include "src/common/DTO.h"
#include "src/common/constants.h"
#include <sstream>

Gameloop::Gameloop(Monitor& _monitor,const std::string& gid, std::string map_name, const int client_id):
        game_id(gid), monitor(_monitor), owner_id(client_id), ready_to_start(false), race(select_track_yaml(map_name), &cars) {
    initialize_car_actions();
    world.generate_map(map_name);

    if (map_name == "San Andreas") {
        current_map = SAN_ANDREAS;
    } else if (map_name == "Vice City") {
        current_map = VICE_CITY;
    } else {
        current_map = LIBERTY_CITY;
    }
    current_phase = std::make_unique<Lobby>(this, 1.0f);
}

std::string Gameloop::select_track_yaml(const std::string& map_name) {
    if (map_name == "San Andreas") {
        return "../src/server/tracks/prueba.yaml";
    }
    else if (map_name == "Vice City") {
        return "../src/server/tracks/vice_city_default.yaml";
    }else {
        return "../src/server/tracks/san_andreas_default.yaml"; 
    }
}

void Gameloop::initialize_car_actions() {
    car_actions[SEND_ACCELERATE] = [](Car& car) { car.accelerate(); };
    car_actions[SEND_ROTATE_RIGHT] = [](Car& car) { car.turn_right(); };
    car_actions[SEND_ROTATE_LEFT] = [](Car& car) { car.turn_left(); };
    car_actions[SEND_BRAKE] = [](Car& car) { car.brake(); };
    car_actions[SEND_USE_NITRO] = [](Car& car) { car.toggle_nitro_status(); };

    car_actions[SEND_WIN_RACE_CHEAT] = [](Car& car) { car.activate_win_race(); };
    car_actions[SEND_RESTORE_LIFE_CHEAT] = [](Car& car) { car.restore_life(); };
    car_actions[SEND_INFINITE_LIFE_CHEAT] = [](Car& car) { car.activate_infinite_life(); };
    car_actions[SEND_LOSE_RACE_CHEAT] = [](Car& car) { car.activate_lose_race(); };
    car_actions[SEND_INFINITE_NITRO_CHEAT] = [](Car& car) { car.activate_infinite_nitro(); };
}

void Gameloop::run() {
    size_t cont = 0;
    while (this->should_keep_running()) {
        try {
            if (cont > MAX_RACES){
                break;
            }            
            current_phase->run();
        }catch (...) {
            break; // deberia ser para cuando se queda sin jugadores la partida o algo asi
        }

        cont++;
    }
}

void Gameloop::process_command(ClientCommand& client_command) {
    if (client_command.cmd_struct.cmd == SEND_DISCONNECT) {
        user_names.erase(client_command.id);
        cars.erase(client_command.id);
        return;
    }

    auto it = cars.find(client_command.id);
    //auto& id = it->first;
    /*if (race.car_finished(id) || race.car_dead(id)) {
        return;
    }*/

    Car& car = it->second;
    auto action = car_actions.find(client_command.cmd_struct.cmd);
    if (action != car_actions.end()) {
        action->second(car);
    }

}

bool Gameloop::is_username_taken(const int username_id) const {
    return user_names.find(username_id) != user_names.end();
}

void Gameloop::add_car(const int client_id, const int car_id,  const std::string& player_name) {
    auto [mass, handling, acceleration, braking] = parser.parse_car(car_id);
    user_names[client_id] = player_name;

    cars.emplace(std::piecewise_construct,
    std::forward_as_tuple(client_id),
    std::forward_as_tuple(world.get_id(), mass, handling, acceleration, braking, car_id));
}

void Gameloop::push_command(const ClientCommand& cmd){
    this->cmd_queue.push(cmd);
}

void Gameloop::update_positions() {
    for (auto& [id, car] : cars) {
        car.update_nitro_usage();
        car.apply_nitro_force();
    }
    world.update();
}

Snapshot Gameloop::initialize_DTO() {
    Snapshot dto;
    dto.game_id = std::stoi(this->game_id);
    dto.cars_count = static_cast<int>(cars.size());
    dto.map = current_map;
    dto.upgrade = NONE_UPGRADE;
    dto.upgradeable = false;
    dto.collision = NONE_COLLISION;
    dto.lobby_cars = {};
    dto.checkpoint = {0,0};
    dto.hint = {0,0,0};
    dto.position = 0;
    dto.type_checkpoint = REGULAR;
    dto.id = -1;
    dto.time_ms = -1;
    dto.state = IN_LOBBY;
    dto.is_owner = false;

    return dto;
}

void Gameloop::broadcast_lobby(const int time_ms) {
    std::unordered_map<int, CarDTO> carsDTO;
    for  (auto& [id, car] : cars) {
        CarDTO car_dto = car.get_state();
        car_dto.state = IN_GAME;
        carsDTO.emplace(id, car_dto);
    }
    for (auto& [id, car] : cars) {
        Snapshot dto = initialize_DTO();
        dto.cars = carsDTO;
        dto.state = IN_LOBBY;
        dto.is_owner = id == owner_id;
        dto.time_ms = time_ms;
        monitor.broadcast(dto,this->game_id, id);
    }
}

void Gameloop::broadcast_in_game(const int time_ms) {
    std::unordered_map<int, CarDTO> carsDTO;
    for  (auto& [id, car] : cars) {
        CarDTO car_dto = car.get_state();
        car_dto.state = race.get_state(id,time_ms);
        carsDTO.emplace(id, car_dto);
    }
    for (auto& [id, car] : cars) {
        Snapshot dto = initialize_DTO();
        dto.cars = carsDTO;
        dto.checkpoint = race.get_checkpoint(id);
        dto.hint = race.get_hint(id);
        dto.position = race.get_position(id);
        dto.type_checkpoint = race.get_cp_type(id);
        dto.state = IN_RACE;
        dto.is_owner = carsDTO[id].car_id == owner_id;
        dto.time_ms = time_ms;
        monitor.broadcast(dto,this->game_id, id);
    }
}

void Gameloop::broadcast_workshop(std::map<Upgrades, std::chrono::seconds> prices, const int time_ms ) {
    std::unordered_map<int, CarDTO> carsDTO;
    for  (auto& [id, car] : cars) {
        CarDTO car_dto = car.get_state();
        car_dto.state = FINISHED;
        carsDTO.emplace(id, car_dto);
    }
    for (auto& [id, car] : cars) {
        Snapshot dto = initialize_DTO();
        dto.cars = carsDTO;
        dto.is_owner = id == owner_id;
        dto.state = IN_WORK_SHOP;
        dto.prices = prices;
        dto.time_ms = time_ms;
        monitor.broadcast(dto,this->game_id, id);
    }
}

bool Gameloop::can_join_to_game(){ return cars.size() < MAX_PLAYERS_PER_GAME; }

const std::string& Gameloop::get_game_id() const{ return this->game_id; }

bool Gameloop::try_pop(ClientCommand& command) { return cmd_queue.try_pop(command); }

bool Gameloop::is_game_already_started() const { return this->ready_to_start; }

void Gameloop::accelerate_upgrade(int& id) {
    auto it = cars.find(id);
    Car& car = it->second;
    car.accelerate_upgrade();
}

void Gameloop::handling_upgrade(int& id) {
    auto it = cars.find(id);
    Car& car = it->second;
    car.handling_upgrade();
}

void Gameloop::nitro_upgrade(int& id) {
    auto it = cars.find(id);
    Car& car = it->second;
    car.nitro_upgrade();
}

void Gameloop::life_upgrade(int& id) {
    auto it = cars.find(id);
    Car& car = it->second;
    car.life_upgrade();
}

void Gameloop::brake_upgrade(int& id) {
    auto it = cars.find(id);
    Car& car = it->second;
    car.brake_upgrade();
}

void Gameloop::mass_upgrade(int& id) {
    auto it = cars.find(id);
    Car& car = it->second;
    car.mass_upgrade();
}

bool Gameloop::is_running() const { return should_keep_running(); }

void Gameloop::handle_lobby_command(const ClientCommand& cmd) {
    if (cmd.cmd_struct.cmd == SEND_READY_TO_PLAY && cmd.id == owner_id) {
        ready_to_start = true;
    }
}

void Gameloop::change_phase(std::unique_ptr<Phase> new_phase) {this->current_phase = std::move(new_phase); }


bool Gameloop::did_all_finish() { //deberiamos validar q todos los jugadores vivoss lleguen  a al meta
    int cars_finished = 0;
    for (auto& [id, car] : cars) {
        if (race.car_finished(id)) {
            cars_finished++;
        }
    }
    return cars_finished == (int)cars.size();
}

void Gameloop::start_race() {
    Checkpoint start = race.get_start();
    float start_angle = race.get_start_angle();
    for (auto& [id, car] : cars) {
        car.set_spawn(start.x,start.y, start_angle);
    }
    update_positions();
    broadcast_in_game(MAX_TIME_PER_RACE);
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

void Gameloop::update_race_state() {
    race.update_checkpoints();
    race.update_positions_order();
}