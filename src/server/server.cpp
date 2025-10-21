#include "server.h"

#include <cstdint>
#include <iostream>

#include "../common/constants.h"

#include "client_acceptor.h"
#include "monitor.h"

Server::Server(const std::string& port): port(port){}

void Server::run() {
    Monitor monitor;
    ClientAcceptor acceptor(port, monitor);
    std::cout << "[SERVER] Listening on port " << port << std::endl;

    std::cout << "[SERVER] Acceptor started " << std::endl;
    acceptor.start();
    std::string line;
    while (std::getline(std::cin, line)) {
        if (!line.empty() && line[0] == SERVER_CLOSE) {
            break;
        }
    }

    // while (true) {
    //     Socket client_socket = server_socket.accept();
    //     std::cout << "[SERVER] Client connected" << std::endl;

    //     ServerProtocol protocol(client_socket);

    //     // Loop para procesar múltiples mensajes del mismo cliente
    //     bool keep_running = true;
    //     while (keep_running) {
    //         std::string message = protocol.receive_message();

    //         if (message.empty()) {
    //             std::cout << "[SERVER] Client disconnected" << std::endl;
    //             break;
    //         }

    //         std::cout << "[SERVER] Received message (size: " << message.size() << ")" << std::endl;

    //         // Verifica el tipo de mensaje
    //         if (!message.empty()) {
    //             uint8_t msg_type = static_cast<uint8_t>(message[0]);
    //             std::cout << "[SERVER] Message type: " << static_cast<int>(msg_type) << std::endl;

    //             // Si es QUIT, termina el loop
    //             if (msg_type == SEND_QUIT) {
    //                 std::cout << "[SERVER] Client sent QUIT" << std::endl;
    //                 protocol.send_message("Goodbye");
    //                 keep_running = false;
    //             } else {
    //                 std::string command_name;

    //                 switch (msg_type) {
    //                     case SEND_CONNECT:
    //                         command_name = "CONNECT";
    //                         break;
    //                     case SEND_SELECT_CAR:
    //                         command_name = "SELECT_CAR";
    //                         break;
    //                     case SEND_READY_TO_PLAY:
    //                         command_name = "READY_TO_PLAY";
    //                         break;
    //                     case SEND_ACCELERATE:
    //                         command_name = "ACCELERATE";
    //                         break;
    //                     case SEND_ROTATE_RIGHT:
    //                         command_name = "ROTATE_RIGHT";
    //                         break;
    //                     case SEND_ROTATE_LEFT:
    //                         command_name = "ROTATE_LEFT";
    //                         break;
    //                     case SEND_BRAKE:
    //                         command_name = "BRAKE";
    //                         break;
    //                     case SEND_USE_NITRO:
    //                         command_name = "USE_NITRO";
    //                         break;
    //                     case SEND_LIFE_UPGRADE:
    //                         command_name = "LIFE_UPGRADE";
    //                         break;
    //                     case SEND_VELOCITY_UPGRADE:
    //                         command_name = "VELOCITY_UPGRADE";
    //                         break;
    //                     case SEND_ACCELERATION_UPGRADE:
    //                         command_name = "ACCELERATION_UPGRADE";
    //                         break;
    //                     case SEND_HANDLING_UPGRADE:
    //                         command_name = "HANDLING_UPGRADE";
    //                         break;
    //                     case SEND_CONTROL_UPGRADE:
    //                         command_name = "CONTROL_UPGRADE";
    //                         break;
    //                     case SEND_WIN_RACE_CHEAT:
    //                         command_name = "WIN_RACE_CHEAT";
    //                         break;
    //                     case SEND_RESTORE_LIFE_CHEAT:
    //                         command_name = "RESTORE_LIFE_CHEAT";
    //                         break;
    //                     case SEND_INFINITE_LIFE_CHEAT:
    //                         command_name = "INFINITE_LIFE_CHEAT";
    //                         break;
    //                     case SEND_LOSE_RACE_CHEAT:
    //                         command_name = "LOSE_RACE_CHEAT";
    //                         break;
    //                     case SEND_INFINITE_NITRO_CHEAT:
    //                         command_name = "INFINITE_NITRO_CHEAT";
    //                         break;
    //                     default:
    //                         command_name = "UNKNOWN";
    //                         break;
    //                 }

    //                 std::cout << "[SERVER] Received command: " << command_name
    //                           << " (code: " << static_cast<int>(msg_type) << ")" << std::endl;

    //                 protocol.send_message("RECIBIDO POR EL SERVER: " + command_name);
    //             }
    //         }
    //     }
    // }
    std::cout << "[SERVER] Acceptor close" << std::endl;
    acceptor.close_acceptor_socket();
    acceptor.join();
    std::cout << "[SERVER] Connection closed" << std::endl;
}