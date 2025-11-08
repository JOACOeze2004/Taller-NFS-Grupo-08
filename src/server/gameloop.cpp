#include "gameloop.h"

#include "src/common/DTO.h"
#include "src/common/constants.h"

Gameloop::Gameloop(Monitor& _monitor,const std::string& gid, std::string map_name):
        cmd_queue(),game_id(gid), monitor(_monitor), ready_to_start(false) {
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
            for (auto& [id, car] : cars) {
                car.update_nitro_usage();
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

        if (client_command.cmd_struct.cmd == 0) {
            cars.erase(client_command.id);
            continue;
        }

        auto it = cars.find(client_command.id);
        if (it == cars.end()) {
            add_car(client_command.id);
        }
        it = cars.find(client_command.id);
        Car& car = it->second;
        auto action = car_actions.find(client_command.cmd_struct.cmd);
        if (action != car_actions.end()) {
            action->second(car);
        }
    }
}

void Gameloop::add_car(const int client_id) {
    if (cars.size() == MAX_PLAYERS_PER_GAME){
        //estaria bueno mandar un command especifico diciendole al jugador q no se pudo unir xq estaba llena la partida. O tirar una excepcion (no me gusta mucho eso)
        std::cout << "[GAMELOOP] The game is already full.Try to join to other game." << std::endl;        
        return;
    }    
    cars.emplace(std::piecewise_construct,
    std::forward_as_tuple(client_id),
    std::forward_as_tuple(world.get_id()));
}

void Gameloop::push_command(const ClientCommand& cmd){
    this->cmd_queue.push(cmd);
}

void Gameloop::update_positions() {
    world.update();
}

void Gameloop::broadcast() const {
    std::unordered_map<int, CarDTO> carsDTO;
    for  (auto& [id, car] : cars) {
        carsDTO.emplace(id, car.get_state());
    }
    Snapshot dto; 
    dto.cars = std::move(carsDTO);
    dto.state = IN_GAME;
    dto.position = 1;   //cambiar Lo q este estatico a dinamico cuando se tenga (posicion, mejoras, checkpoints, etc) 
    dto.cars_count = cars.size();
    dto.map = current_map;
    dto.upgrade = NONE_UPGRADE;
    dto.upgradeable = false;
    dto.collision = NONE_COLLISION;
    dto.checkpoint = {0.0f, 0.0f};
    dto.hint = {0.0f, 0.0f, 0.0f};
    dto.type_checkpoint = REGULAR;
    dto.time_ms = get_time_remaining_ms();
    
    monitor.broadcast(dto,this->game_id);
}

bool Gameloop::can_join_to_game(){ return cars.size() < MAX_PLAYERS_PER_GAME; }

const std::string& Gameloop::get_game_id() const{ return this->game_id; }
