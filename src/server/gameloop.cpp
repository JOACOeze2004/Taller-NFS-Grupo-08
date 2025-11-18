#include "gameloop.h"

#include "src/common/DTO.h"
#include "src/common/constants.h"
#include <sstream>

Gameloop::Gameloop(Monitor& _monitor,const std::string& gid, std::string map_name, const int client_id):
        game_id(gid), monitor(_monitor), owner_id(client_id), ready_to_start(false), race(map_name, &cars, TRACKS_PATH), game_started(false) {
    initialize_car_actions();
    world.generate_map(map_name);

    if (map_name == SAN_ANDREAS_STR ) {
        current_map = SAN_ANDREAS;
    } else if (map_name == VICE_CITY_STR) {
        current_map = VICE_CITY;
    } else {
        current_map = LIBERTY_CITY;
    }
    current_phase = std::make_unique<Lobby>(this, 1.0f);
}

void Gameloop::initialize_car_actions() {
    car_actions[SEND_ACCELERATE] = [](Car& car) { car.accelerate(); };
    car_actions[SEND_ROTATE_RIGHT] = [](Car& car) { car.turn_right(); };
    car_actions[SEND_ROTATE_LEFT] = [](Car& car) { car.turn_left(); };
    car_actions[SEND_BRAKE] = [](Car& car) { car.brake(); };
    car_actions[SEND_USE_NITRO] = [](Car& car) { car.toggle_nitro_status(); };

    car_actions[SEND_RESTORE_LIFE_CHEAT] = [](Car& car) { car.restore_life(); };
    car_actions[SEND_INFINITE_LIFE_CHEAT] = [](Car& car) { car.activate_infinite_life(); };
    car_actions[SEND_LOSE_RACE_CHEAT] = [](Car& car) { car.activate_lose_race(); };
    car_actions[SEND_INFINITE_NITRO_CHEAT] = [](Car& car) { car.activate_infinite_nitro(); };

    race_actions[SEND_WIN_RACE_CHEAT] = [](Race& race, int& id) { race.activate_win(id); };
}

void Gameloop::run() {
    while (this->should_keep_running()) {
        try {
            current_phase->run();
        }catch (...) {
            break; // deberia ser para cuando se queda sin jugadores la partida o algo asi
        }
    }
}

void Gameloop::process_command(ClientCommand& client_command) {
    if (client_command.cmd_struct.cmd == SEND_DISCONNECT) {
        user_names.erase(client_command.id);
        cars.erase(client_command.id);
        return;
    }

    auto it = cars.find(client_command.id);
    int id = it->first;
    if (race.car_finished(id) || race.car_dead(id)) {
        return;
    }

    Car& car = it->second;
    auto action = car_actions.find(client_command.cmd_struct.cmd);
    if (action != car_actions.end()) {
        action->second(car);
    }
    else {
        auto race_action = race_actions.find(client_command.cmd_struct.cmd);
        if (race_action != race_actions.end()) {
            race_action->second(race, id);
        }
    }

}

bool Gameloop::is_username_taken(const int username_id) const {
    return user_names.find(username_id) != user_names.end();
}

void Gameloop::add_car(const int client_id, const int car_id, const std::string& player_name) {
    auto [mass, handling, acceleration, braking] = parser.parse_car(car_id);
    user_names[client_id] = player_name;
    player_total_times[client_id] = 0;

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
    dto.cars_finished = finished;
    dto.cars_finished.insert(dto.cars_finished.end(), deads.begin(), deads.end());
    dto.player_total_times = player_total_times;
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

bool Gameloop::can_join_to_game(){ return cars.size() < MAX_PLAYERS_PER_GAME && !game_started; }

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

void Gameloop::accelerate_downgrade(int& id) {
    auto it = cars.find(id);
    Car& car = it->second;
    car.accelerate_downgrade();
}

void Gameloop::handling_downgrade(int& id) {
    auto it = cars.find(id);
    Car& car = it->second;
    car.handling_downgrade();
}

void Gameloop::nitro_downgrade(int& id) {
    auto it = cars.find(id);
    Car& car = it->second;
    car.nitro_downgrade();
}

void Gameloop::life_downgrade(int& id) {
    auto it = cars.find(id);
    Car& car = it->second;
    car.life_downgrade();
}

void Gameloop::brake_downgrade(int& id) {
    auto it = cars.find(id);
    Car& car = it->second;
    car.brake_downgrade();
}

void Gameloop::mass_downgrade(int& id) {
    auto it = cars.find(id);
    Car& car = it->second;
    car.mass_downgrade();
}

bool Gameloop::is_running() const { return should_keep_running(); }

void Gameloop::handle_lobby_command(const ClientCommand& cmd) {
    if (cmd.cmd_struct.cmd == SEND_READY_TO_PLAY && cmd.id == owner_id) {
        ready_to_start = true;
    }
}

void Gameloop::change_phase(std::unique_ptr<Phase> new_phase) {
    this->current_phase = std::move(new_phase);
    finished.clear();
    deads.clear();
}


bool Gameloop::did_all_finish() {
    int cars_finished = 0;
    for (auto& [id, car] : cars) {
        if (race.car_finished(id)) {
            cars_finished++;
        }
    }
    return cars_finished == (int)cars.size();
}

void Gameloop::start_race() {
    game_started = true;
    Checkpoint start = race.get_start();
    Checkpoint start_angle = race.get_start_angle();
    for (auto& [id, car] : cars) {
        car.set_spawn(start.x,start.y, start_angle.x, start_angle.y);
    }
    update_positions();
    broadcast_in_game(MAX_TIME_PER_RACE);
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

void Gameloop::update_race_state() {
    race.update_checkpoints();
    race.update_positions_order();
    for (auto& [id, car] : cars) {
        auto it = std::ranges::find_if(
                finished, [&] (const CarRacingInfo& player) { return player.name == user_names[id];});
        bool not_added = it == finished.end();
        if (race.car_finished(id) && not_added) {
            int race_time = current_phase->get_time();
            finished.emplace_back(user_names[id], race_time, race.get_position(id));
            player_total_times[id] += race_time;
            continue;
        }
        it = std::ranges::find_if(
                deads, [&] (const CarRacingInfo& player) { return player.name == user_names[id];});
        not_added = it == deads.end();
        if (race.car_dead(id) && not_added) {
            deads.emplace_back(user_names[id], current_phase->get_time(), -1);
            player_total_times[id] += (MAX_TIME_PER_RACE / 1000);
        }
    }
}

FinalScoreList Gameloop::calculate_final_results() {
    FinalScoreList results;

    for (const auto& [client_id, total_time] : player_total_times) {
        CarRacingInfo result;
        result.name = user_names[client_id];
        result.time = static_cast<float>(total_time);
        result.position = 0;
        results.push_back(result);
    }
    std::sort(results.begin(), results.end(),
              [](const CarRacingInfo& a, const CarRacingInfo& b) { return a.time < b.time; });

    for (size_t i = 0; i < results.size(); ++i) {
        results[i].position = i + 1;
    }

    return results;
}

void Gameloop::broadcast_final_results(const FinalScoreList& results) {
    std::unordered_map<int, CarDTO> carsDTO;
    for (auto& [id, car] : cars) {
        CarDTO car_dto = car.get_state();
        car_dto.state = FINISHED;
        carsDTO.emplace(id, car_dto);
    }

    for (auto& [id, car] : cars) {
        Snapshot dto = initialize_DTO();
        dto.cars = carsDTO;
        dto.is_owner = id == owner_id;
        dto.state = FINAL_RESULTS;
        dto.time_ms = 0;
        monitor.broadcast(dto, this->game_id, id);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    monitor.broadcast_final_results(results, game_id);
}

void Gameloop::increment_race_counter() {
    races_completed++;
    std::cout << "[GAMELOOP] Races completed: " << races_completed << "/" << MAX_RACES << std::endl;
}

int Gameloop::get_races_completed() const { return races_completed;}

void Gameloop::reset_race() {
    std::cout << "[GAMELOOP] Resetting race for next round" << std::endl;
    race.reset_race();
}

bool Gameloop::has_active_players() const { return !cars.empty();}