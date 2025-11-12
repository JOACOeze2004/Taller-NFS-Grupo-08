#include "lobby.h"
#include "gameloop.h"

Lobby::Lobby(Gameloop* _gameloop, float _duration): Phase(_gameloop, _duration) {}

void Lobby::run() {
    auto rate = std::chrono::milliseconds(16);
    auto t1 = std::chrono::steady_clock::now();
    while (should_continue()) {

        ClientCommand command{};
        while (gameloop->try_pop(command)) {
            execute(command);
            gameloop->process_command(command);
        }
        gameloop->broadcast_lobby();


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


void Lobby::execute(ClientCommand& command) {
    if (command.id == 0 && command.cmd_struct.cmd == SEND_READY_TO_PLAY) {
        cont = false;
    }
}
