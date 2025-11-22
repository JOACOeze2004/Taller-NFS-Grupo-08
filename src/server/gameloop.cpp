#include "gameloop.h"

#include "src/common/DTO.h"
#include "src/common/constants.h"
#include <sstream>

Gameloop::Gameloop(Monitor& _monitor,const std::string& gid, std::string map_name, const int client_id):
        game_id(gid), monitor(_monitor), owner_id(client_id), ready_to_start(false), race(map_name, &cars, TRACKS_PATH), game_started(false), command_processor() {
    world.generate_map(map_name);
    corners = world.generate_corners(map_name);
    generate_npcs();

    if (map_name == SAN_ANDREAS_STR ) {
        current_map = SAN_ANDREAS;
    } else if (map_name == VICE_CITY_STR) {
        current_map = VICE_CITY;
    } else {
        current_map = LIBERTY_CITY;
    }
    current_phase = std::make_unique<Lobby>(this, 1.0f);
}

void Gameloop::run() {
    while (this->should_keep_running()) {
        try {
            current_phase->run();
        }catch (...) {
            break;
        }
    }
}

void Gameloop::process_command(ClientCommand& client_command) {
    int command = client_command.cmd_struct.cmd;
    int command_id = client_command.id;

    if (command == SEND_DISCONNECT) {
        user_names.erase(command_id);
        cars.erase(command_id);
        return;
    }

    auto it = cars.find(command_id);
    int id = it->first;
    if (race.car_finished(id) || race.car_dead(id)) {
        return;
    }

    Car& car = it->second;
    if (auto action = command_processor.get_car_action(command)) {
        (*action)(car);
        return;
    }
    if (auto action = command_processor.get_race_action(command)) {
        (*action)(race, id);
        return;
    }
}

void Gameloop::generate_npcs() {
    for (int i =0; i < 30; i++) {
        auto rand_corner = rand() % corners.size();
        npcs.emplace_back(world.get_id(), &corners, rand_corner);
    }
}

bool Gameloop::is_username_taken(const int username_id) const { return user_names.find(username_id) != user_names.end(); }

void Gameloop::add_car(const int client_id, const int car_id, const std::string& player_name) {
    auto [mass, handling, acceleration, braking] = parser.parse_car(car_id);
    user_names[client_id] = player_name;
    results.initialize_player(client_id);

    cars.emplace(std::piecewise_construct,
    std::forward_as_tuple(client_id),
    std::forward_as_tuple(world.get_id(), mass, handling, acceleration, braking, car_id));
}
void Gameloop::push_command(const ClientCommand& cmd) { this->cmd_queue.push(cmd); }

void Gameloop::update_positions() {
    race.update_cars();
    world.update();
    for (auto& npc : npcs) {
        npc.update();
    }
}

std::unordered_map<int, CarDTO> Gameloop::build_cars_dto(std::function<StateRunning(int)> car_state){
    std::unordered_map<int, CarDTO> cars_DTO;
    for  (auto& [id, car] : cars) {
        CarDTO car_dto = car.get_state();
        car_dto.state = car_state(id);
        cars_DTO.emplace(id, car_dto);
    }

    for (auto& npc : npcs) {
        CarDTO car_dto = npc.get_state();
        cars_DTO.emplace(-1, car_dto);
    }

    return cars_DTO;
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
    dto.cars_finished = results.get_finished();
    auto deads = results.get_deads();
    dto.cars_finished.insert(dto.cars_finished.end(), deads.begin(), deads.end());
    dto.player_total_times = results.get_total_times();
    return dto;
}

Snapshot Gameloop::build_base_snapshot( const std::unordered_map<int, CarDTO>& cars_DTO,State state,int time_ms, int player_id) {
    Snapshot dto = initialize_DTO();
    dto.cars = cars_DTO;
    dto.state = state;
    dto.time_ms = time_ms;
    dto.is_owner = player_id == owner_id;
    return dto;
}

void Gameloop::broadcast_lobby(const int time_ms) {
    common_broadcast( IN_LOBBY ,time_ms, 
        [](int){ return StateRunning::IN_GAME;},
        [](Snapshot&,int) {}
    );
}

void Gameloop::broadcast_in_game(const int time_ms) {
    common_broadcast( IN_RACE ,time_ms, 
        [&](int id){ return race.get_state(id, time_ms); },
        [&](Snapshot& dto,int id) {
            dto.checkpoint = race.get_checkpoint(id);
            dto.hint = race.get_hint(id);
            dto.position = race.get_position(id);
            dto.type_checkpoint = race.get_cp_type(id);
        }    
    );
}

void Gameloop::broadcast_workshop(std::map<Upgrades, std::chrono::seconds> prices, const int time_ms ) {
    common_broadcast( IN_WORK_SHOP ,time_ms, 
        [](int){ return StateRunning::FINISHED;},
        [prices](Snapshot& dto,int) {
            dto.prices = prices;
        }
    );
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
    results.reset_for_next_race(); 
}

bool Gameloop::did_all_finish() {return race.all_cars_done(); }

void Gameloop::start_race() {
    game_started = true;
    race.spawn_cars();
    update_positions();
    broadcast_in_game(MAX_TIME_PER_RACE);
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

void Gameloop::update_race_state() {
    race.update_checkpoints();
    race.update_positions_order();
    for (auto& [id, car] : cars) {
        if (race.car_finished(id)) {
            results.add_finished(id, user_names[id], current_phase->get_time(), race.get_position(id));
        }
        if (race.car_dead(id)) {
            results.add_dead(id, user_names[id], current_phase->get_time(), MAX_TIME_PER_RACE / 1000);
        }
    }
}

FinalScoreList Gameloop::calculate_final_results() { return results.calculate_final_scores(user_names); }

void Gameloop::broadcast_final_results(const FinalScoreList& results) {
    common_broadcast( IN_LOBBY ,0, 
        [](int){ return StateRunning::FINISHED;},
        [](Snapshot&,int) {}   
    );
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    monitor.broadcast_final_results(results, game_id);
}

void Gameloop::increment_race_counter() { races_completed++; }

int Gameloop::get_races_completed() const { return races_completed;}

void Gameloop::reset_race() { race.reset_race(); }

bool Gameloop::has_active_players() const { return !cars.empty();}