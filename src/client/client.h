#ifndef TALLER_TP_CLIENT_H
#define TALLER_TP_CLIENT_H

#include <iostream>
#include <string>
#include <SDL2/SDL.h>

#include "../common/socket.h"
#include "pantallas/login_window.h"

#include "client_protocol.h"
#include "client_receiver.h"
#include "client_sender.h"
#include "client_handler.h"
#include "graphic_client.h"
#include "../common/queue.h"
#include "client_car.h"
#include "ClientQuitException.h"
#include "../common/DTO.h"
#include "audio_manager.h"



class Client {
public:
    static constexpr Uint32 TARGET_FPS = 60;
    static constexpr Uint32 FRAME_DELAY_MS = 1000 / TARGET_FPS;
    static constexpr Uint32 SNAPSHOT_POLL_DELAY_MS = 10;

    Client(const std::string& host, const std::string& port, AudioManager* audio);
    void run();
    bool has_final_results() const;
    FinalScoreList get_final_results() const;
    void send_config(const PlayerConfig& config,uint8_t lobby_action, const std::string& game_id);
    void wait_lobby();

private:
    std::string host;
    std::string port;
    Socket client_socket;
    ClientProtocol protocol;
    FinalScoreList final_results;
    bool final_results_received;
    AudioManager* audio_manager;

    bool initialize_game_data();
    bool wait_for_initial_snapshot(ClientReceiver& receiver, Snapshot& snapshot);
    void game_loop(ClientReceiver& receiver, ClientHandler& handler, 
                   GraphicClient& graphic_client, Snapshot& snapshot);
    bool should_exit() const;
    bool update_game_state(ClientReceiver& receiver, GraphicClient& graphic_client, Snapshot& snapshot);
    bool handle_input(ClientHandler& handler);
    void render_frame(GraphicClient& graphic_client, Snapshot& snapshot);
    void limit_frame_rate(Uint32 frame_start);
    void cleanup_resources(ClientReceiver& receiver, ClientSender& sender, Queue<Command>& command_queue);
};

#endif  // TALLER_TP_CLIENT_H
