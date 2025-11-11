#include "gameloop.h"

#include "src/common/DTO.h"
#include "src/common/constants.h"
#include "GameFullException.h"
#include <sstream>


Gameloop::Gameloop(Monitor& _monitor,const std::string& gid, std::string map_name):
        cmd_queue(), game_id(gid), monitor(_monitor), ready_to_start(false), race("../src/server/prueba.yaml", &cars) {
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
    auto rate = std::chrono::milliseconds(16);
    auto t1 = std::chrono::steady_clock::now();

    while (should_keep_running()) {
        process_commands();
        //Por ahora, vi q hacen una fase (del phase_handler) para esperar a los jugadores en una sala
        if (!this->ready_to_start && cars.size() == MIN_PLAYERS_TO_START){
            ready_to_start = true;
            start_time = std::chrono::steady_clock::now();
        }
        if (ready_to_start){
            update_positions();
            delete_deads();

            for (auto& [id, car] : cars) {
                car.update_nitro_usage();
                car.apply_nitro_force();
            }
            broadcast();
            if (get_time_remaining_ms() <= 0){
                //deberiamos cambiar de fase carrera a mejora el auto y vovler a empezar otra carrera, pero x ahora, q reinicie el reloj.
                start_time = std::chrono::steady_clock::now();
            }
        }

        auto t2 = std::chrono::steady_clock::now();
        auto rest = rate - std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
        if (rest < std::chrono::milliseconds(0)) {
            auto behind = -rest;
            rest = rate - behind % rate;
            auto lost = behind + rest;
            t1 += lost;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(rest));
        t1 += rate;
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

void Gameloop::process_commands() {
    ClientCommand client_command{};
    while (cmd_queue.try_pop(client_command)) {
        if (client_command.cmd_struct.cmd == SEND_DISCONNECT) {    
            user_names.erase(client_command.id);       
            cars.erase(client_command.id);
            continue;
        }

        auto it = cars.find(client_command.id);
        auto& id = it->first;
        if (race.car_finished(id) || race.car_dead(id)) {
            continue;
        }

        Car& car = it->second;
        auto action = car_actions.find(client_command.cmd_struct.cmd);
        if (action != car_actions.end()) {
            action->second(car);
        }
    }
}

bool Gameloop::is_username_taken(const int username_id) const {
    return user_names.find(username_id) != user_names.end();
}

void Gameloop::add_car(const int client_id, const int car_id,  const std::string& player_name) {
    CarStats car = parser.parse_car(car_id);
    user_names[client_id] = player_name;;

    cars.emplace(std::piecewise_construct,
    std::forward_as_tuple(client_id),
    std::forward_as_tuple(world.get_id(), car.mass, car.handling, car.acceleration, car.braking, car_id));
}

void Gameloop::push_command(const ClientCommand& cmd){
    this->cmd_queue.push(cmd);
}

void Gameloop::update_positions() {
    world.update();
    race.update_checkpoints();
    race.update_positions_order();
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

    return dto;
}

void Gameloop::broadcast() {
    std::unordered_map<int, CarDTO> carsDTO;
    for  (auto& [id, car] : cars) {
        CarDTO car_dto = car.get_state();
        car_dto.state = race.get_state(id, get_time_remaining_ms());
        carsDTO.emplace(id, car_dto);
    }
    for (auto& [id, car] : cars) {
        Snapshot dto = initialize_DTO();
        dto.cars = carsDTO;
        dto.checkpoint = race.get_checkpoint(id);
        dto.hint = race.get_hint(id);
        dto.position = race.get_position(id);
        dto.type_checkpoint = race.get_cp_type(id);
        monitor.broadcast(dto,this->game_id, id);
    }

}

void Gameloop::delete_deads() {
    for (auto& [id, car] : cars) {
        if (car.is_dead() || race.car_finished(id)) {
            car.delete_from_map();
        }
    }
}

bool Gameloop::can_join_to_game(){ return cars.size() < MAX_PLAYERS_PER_GAME; }

const std::string& Gameloop::get_game_id() const{ return this->game_id; }
