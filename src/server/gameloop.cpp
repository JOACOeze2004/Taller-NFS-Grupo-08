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
            if (!has_active_players() || get_races_completed() >= Config::instance().game.max_races){
                break;
            }
            
        }catch (...) {
            break;
        }
    }
}

bool Gameloop::handle_disconnect(const int command,const int id) {
    if (command != SEND_DISCONNECT){
        return false;
    } 
    user_names.erase(id);
    cars.erase(id);
    return true;
}

bool Gameloop::handle_upgrade(const int command, Car& car, const int player_id) {
    if (!upgrader.is_upgrade_command(command)) {
        return false;
    }

    Upgrades type = upgrader.get_upgrade_type(command);
    bool is_upgrade = upgrader.is_upgrade_operation(command);
    
    int price_ms = std::chrono::duration_cast<std::chrono::seconds>( upgrader.get_price(type) ).count();

    if (results.get_upgrade_penalty(player_id) + price_ms >= (MAX_TIME_PER_RACE/1000 - 120)) {
        return true;
    }

    bool upgrade_applied = car.apply_upgrade(type, is_upgrade);
    if (upgrade_applied){
        if (is_upgrade) {
            results.add_upgrade_time(player_id, price_ms);
            results.add_upgrade_penalty(player_id, price_ms);
        } else {
            results.subtract_upgrade_time(player_id, price_ms);
            results.subtract_upgrade_penalty(player_id, price_ms);
        }
    }
    return true;
}

bool Gameloop::handle_car_action(const int command, Car& car) {
    if (auto action = command_processor.get_car_action(command)) {
        (*action)(car);
        return true;
    }
    return false;
}

bool Gameloop::handle_race_action(const int command, int id) {
    if (auto action = command_processor.get_race_action(command)) {
        (*action)(race, id);
        return true;
    }
    return false;
}

bool Gameloop::handle_skip_to_last_race_cheat(const int command) { 
    if (command != SEND_PASS_TO_FINAL_RACE){
        return false;
    }   
    races_completed = Config::instance().game.max_races - 2;
    
    for (auto& [key, car] : cars) {
        int id = key;
        race.activate_win(id);
    }
    return true;
}

bool Gameloop::handle_force_start(const int command) { 
    if (command != SEND_FORCE_LOBBY_START){
        return false;
    }   
    ready_to_start = true;
    return true;
}

void Gameloop::process_command(ClientCommand& client_command) {
    int command = client_command.cmd_struct.cmd;
    int player_id = client_command.id;

    if (handle_disconnect(command, player_id) || handle_skip_to_last_race_cheat(command)) {
        return;
    }
    auto it = cars.find(player_id);
    if (it == cars.end()){
        return;
    }
    Car& car = it->second;
    if (handle_upgrade(command, car, player_id)){
        return;
    } 
    if (race.car_finished(player_id) || race.car_dead(player_id) || race.car_timeouted(player_id)) {
        return;
    }
    if (handle_car_action(command, car) || handle_race_action(command, player_id)) {
        return;
    }
}

void Gameloop::generate_npcs() {
    for (int i = 0; i < Config::instance().game.max_npcs; i++) {
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

    for (int i = 0; i < static_cast<int>(npcs.size()); i++) {
        CarDTO car_dto = npcs[i].get_state();
        cars_DTO.emplace(i+8, car_dto);
    }

    return cars_DTO;
}

Car* Gameloop::find_car(int id) {
    if (auto it = cars.find(id); it != cars.end()){
        return &it->second;
    }
    return nullptr;
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
    dto.total_checkpoints = 0;
    dto.current_checkpoint = 0;
    dto.upgrade_penalty_seconds = 0;
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
        [&](int id){ int player_time_ms = current_phase->get_time_remaining_ms(MAX_TIME_PER_RACE, id);
            return race.get_state(id, player_time_ms); },
        [&](Snapshot& dto,int id) {
            int player_time_ms = current_phase->get_time_remaining_ms(MAX_TIME_PER_RACE, id);
            dto.time_ms = player_time_ms;
            dto.checkpoint = race.get_checkpoint(id);
            dto.hint = race.get_hint(id);
            dto.position = race.get_position(id);
            dto.type_checkpoint = race.get_cp_type(id);
            dto.total_checkpoints = race.get_checkpoint_amount();
            dto.current_checkpoint = race.get_checkpoints(id);
            if (Car* car = find_car(id)) {
                dto.upgrades = car->get_upgrades();
            }
            dto.upgrade_penalty_seconds = results.get_upgrade_penalty(id);
        }    
    );
}

void Gameloop::broadcast_countdown(const int time_ms) {
    common_broadcast( IN_COUNTDOWN ,time_ms, 
        [&](int){ return StateRunning::IN_GAME; },  //VER SI QUEDA INGAME O ALGUN OTRO ESTADO NUEVO
        [&](Snapshot&,int) { }    
    );
}

void Gameloop::broadcast_workshop(const int time_ms ) {
    common_broadcast( IN_WORK_SHOP ,time_ms, 
        [](int){ return StateRunning::FINISHED;},
        [this](Snapshot& dto,int id) {
            dto.prices = upgrader.get_prices();
            if (Car* car = find_car(id)) {
                dto.upgrades = car->get_upgrades();
            }
            int penalty_seconds = results.get_upgrade_penalty(id);
            int final_time_ms = MAX_TIME_PER_RACE - (penalty_seconds * 1000); //Si hace falta, meter alguna validacion de q no pueda ser negativo
            dto.time_ms = final_time_ms;
            dto.upgrade_penalty_seconds = results.get_upgrade_penalty(id);
        }
    );
}

bool Gameloop::can_join_to_game(){ return cars.size() < Config::instance().game.max_players && !game_started; }

const std::string& Gameloop::get_game_id() const{ return this->game_id; }

bool Gameloop::try_pop(ClientCommand& command) { return cmd_queue.try_pop(command); }

bool Gameloop::is_game_already_started() const { return this->ready_to_start; }

bool Gameloop::is_running() const { return should_keep_running(); }

void Gameloop::handle_lobby_command(const ClientCommand& cmd) {
    if (handle_force_start(cmd.cmd_struct.cmd)){
        return;
    }    
    if (cmd.cmd_struct.cmd == SEND_READY_TO_PLAY && cmd.id == owner_id) {
        if (cars.size() < Config::instance().game.min_players){
            return;
        }
        ready_to_start = true;
    }
}

void Gameloop::change_phase(std::unique_ptr<Phase> new_phase) {
    this->current_phase = std::move(new_phase);
    results.reset_for_next_race(); 
}

int Gameloop::get_upgrade_penalty(int player_id) const { return results.get_upgrade_penalty(player_id); }

void Gameloop::reset_upgrade_penalties(){ results.reset_upgrade_penalties(); }

bool Gameloop::did_all_finish() {return race.all_cars_done(); }

void Gameloop::start_race() {
    game_started = true;
    race.spawn_cars();
    update_positions();
    broadcast_in_game(MAX_TIME_PER_RACE);
}

void Gameloop::update_race_state() {
    race.update_checkpoints();
    race.update_positions_order();
    for (auto& [id, car] : cars) {
        if (race.car_finished(id)) {
            results.add_finished(id, user_names[id], current_phase->get_time(), race.get_position(id));
            continue;
        }
         if (race.car_timeouted(id)) {
            results.add_dead(id, user_names[id], current_phase->get_time(), MAX_TIME_PER_RACE / 1000);
            continue;
        }

        if (race.car_dead(id)) {
            results.add_dead(id, user_names[id], current_phase->get_time(), MAX_TIME_PER_RACE / 1000);
        }
    }
}

FinalScoreList Gameloop::calculate_final_results() { return results.calculate_final_scores(user_names); }

void Gameloop::broadcast_final_results(const FinalScoreList& results) {
    common_broadcast( FINAL_RESULTS, 0, 
        [](int){ return StateRunning::FINISHED;},
        [](Snapshot&,int) {}   
    );
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    monitor.broadcast_final_results(results, game_id);
}

void Gameloop::increment_race_counter() { races_completed++; }

int Gameloop::get_races_completed() const { return races_completed;}

void Gameloop::reset_race() { race.reset_race(); }

bool Gameloop::has_active_players() const { return !cars.empty(); }