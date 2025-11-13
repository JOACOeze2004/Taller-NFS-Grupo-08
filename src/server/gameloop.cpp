#include "gameloop.h"

#include "src/common/DTO.h"
#include "src/common/constants.h"
#include "GameFullException.h"
#include <sstream>

Gameloop::Gameloop(Monitor& _monitor,const std::string& gid, std::string map_name):
        game_id(gid), monitor(_monitor), ready_to_start(false), lobby(this, 0.0f), in_game(this, 10.0f, map_name, "../src/server/prueba.yaml", &cars), workshop(this, 10.0f){
    initialize_car_actions();
    world.generate_map(map_name);

    if (map_name == "San Andreas") {
        current_map = SAN_ANDREAS;
    } else if (map_name == "Vice City") {
        current_map = VICE_CITY;
    } else {
        current_map = LIBERTY_CITY;
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
    lobby.run();
    size_t cont = 0;
    while (cont < 5/*MAX_RACES*/) {
        try {
            in_game.run();
            workshop.run();
        }catch (...) {
            break; // deberia ser para cuando se queda sin jugadores la partida o algo asi
        }

        cont++;
    }
}

int Gameloop::get_time_remaining_ms() const {
    if (!this->ready_to_start) {
        return MAX_TIME_PER_RACE;
    }
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();
    return std::max(0, MAX_TIME_PER_RACE - static_cast<int>(elapsed));
}

/*void Gameloop::run() {
    lobby.run();
    size_t cont;
    while (cont < MAX_RACES) {
        in_game.run();
        upgrades.run();
        cont++;
    }
}*/

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
    user_names[client_id] = player_name;;

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
    dto.game_id =std::stoi(this->game_id);
    dto.cars_count = static_cast<int>(cars.size());
    dto.map = current_map;
    dto.upgrade = NONE_UPGRADE;
    dto.upgradeable = false;
    dto.collision = NONE_COLLISION;
    dto.time_ms = get_time_remaining_ms();
    dto.lobby_cars = {};
    dto.checkpoint = {0,0};
    dto.hint = {0,0,0};
    dto.position = 0;
    dto.type_checkpoint = REGULAR;

    return dto;
}

void Gameloop::broadcast_lobby() {
    std::unordered_map<int, CarDTO> carsDTO;
    for  (auto& [id, car] : cars) {
        CarDTO car_dto = car.get_state();
        car_dto.state = IN_GAME;
        carsDTO.emplace(id, car_dto);
    }
    for (auto& [id, car] : cars) {
        Snapshot dto = initialize_DTO();
        dto.cars = carsDTO;
        //dto.state = LOBBY;
        monitor.broadcast(dto,this->game_id, id);
    }
}

void Gameloop::broadcast_in_game() {
    std::unordered_map<int, CarDTO> carsDTO;
    for  (auto& [id, car] : cars) {
        CarDTO car_dto = car.get_state();
        car_dto.state = in_game.get_state(id, get_time_remaining_ms());
        carsDTO.emplace(id, car_dto);
    }
    for (auto& [id, car] : cars) {
        Snapshot dto = initialize_DTO();
        dto.cars = carsDTO;
        dto.checkpoint = in_game.get_checkpoint(id);
        dto.hint = in_game.get_hint(id);
        dto.position = in_game.get_position(id);
        dto.type_checkpoint = in_game.get_cp_type(id);
        monitor.broadcast(dto,this->game_id, id);
    }

}

bool Gameloop::can_join_to_game(){ return cars.size() < MAX_PLAYERS_PER_GAME; }

const std::string& Gameloop::get_game_id() const{ return this->game_id; }

bool Gameloop::try_pop(ClientCommand& command) {
    return cmd_queue.try_pop(command);
}
