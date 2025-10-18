#include "client.h"
#include "client_protocol.h"
#include "../common/constants.h"

Client::Client(const std::string& host, const std::string& port)
    : host(host), port(port), client_socket(host.c_str(), port.c_str()) {}

void Client::run() {
    std::cout << "[CLIENT] Connected to " << host << ":" << port << std::endl;

    ClientProtocol protocol(client_socket);
    protocol.send_message("Hello from client");

    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }

    test_msgs(); // linea a borrar esta solo para demostrar comunicacion entre cliente y server


    client_socket.shutdown(2);
    client_socket.close();
}

void Client::test_msgs() {

    const std::string nombre_usuario = "MeteoroUser";
    const std::string nombre_coche = "Meteoro";

    std::cout << "[CLIENT] Connected to " << host << ":" << port << std::endl;
    ClientProtocol protocol(client_socket);
    
    std::cout << "probando mensajes de cliente a server menu inicial" << std::endl;

    std::string msg_connect;

    msg_connect.push_back(static_cast<char>(SEND_CONNECT)); 



    msg_connect.append(nombre_usuario);

    protocol.send_message(msg_connect);

    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received from connect: " << response << std::endl;
    }

    //mensaje 2
    std::string msg_select_car;
    msg_select_car.push_back(static_cast<char>(SEND_SELECT_CAR));
    msg_select_car.append(nombre_coche);
    
    protocol.send_message(msg_select_car);

    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received from select car: " << response << std::endl;
    }

    //mensaje 3
    std::string msg_ready_to_play;
    msg_ready_to_play.push_back(static_cast<char>(SEND_READY_TO_PLAY));

    protocol.send_message(msg_ready_to_play);
    
    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received from ready to play: " << response << std::endl;
    }

    

    //enviar acelerar 
    std::string msg_accelerate;
    msg_accelerate.push_back(static_cast<char>(SEND_ACCELERATE));
    protocol.send_message(msg_accelerate);

    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }


    //enviar derecha 
    std::string msg_rotate_right;
    msg_rotate_right.push_back(static_cast<char>(SEND_ROTATE_RIGHT));
    protocol.send_message(msg_rotate_right);

    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }

    //enviar acelerar 
    std::string msg_rotate_left;
    msg_rotate_left.push_back(static_cast<char>(SEND_ROTATE_LEFT));
    protocol.send_message(msg_rotate_left);

    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }

    //enviar acelerar 
    std::string msg_brake;
    msg_brake.push_back(static_cast<char>(SEND_BRAKE));
    protocol.send_message(msg_brake);

    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }


    //enviar usar nitro
    std::string msg_use_nitro;
    msg_use_nitro.push_back(static_cast<char>(SEND_USE_NITRO));
    protocol.send_message(msg_use_nitro);

    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }


    //enviar upgrades
    std::string msg_life_upgrade;
    msg_life_upgrade.push_back(static_cast<char>(SEND_LIFE_UPGRADE));
    protocol.send_message(msg_life_upgrade);
    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }

    //mejora velociad
    std::string msg_velocity_upgrade;
    msg_velocity_upgrade.push_back(static_cast<char>(SEND_VELOCITY_UPGRADE));
    protocol.send_message(msg_velocity_upgrade);
    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }

    //mejora aceleracion

    std::string msg_acceleration_upgrade;
    msg_acceleration_upgrade.push_back(static_cast<char>(SEND_ACCELERATION_UPGRADE));
    protocol.send_message(msg_acceleration_upgrade);
    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }


    //mejora handling

    std::string msg_handling_upgrade;
    msg_handling_upgrade.push_back(static_cast<char>(SEND_HANDLING_UPGRADE));
    protocol.send_message(msg_handling_upgrade);
    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }

    //mejora masa
    std::string msg_control_upgrade;
    msg_control_upgrade.push_back(static_cast<char>(SEND_CONTROL_UPGRADE));
    protocol.send_message(msg_control_upgrade);
    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }


    //cheatsssssssss
    //gano carrera
    std::string msg_win_race_cheat;
    msg_win_race_cheat.push_back(static_cast<char>(SEND_WIN_RACE_CHEAT));
    protocol.send_message(msg_win_race_cheat);
    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }

    //restauro vida
    std::string msg_restore_life_cheat;
    msg_restore_life_cheat.push_back(static_cast<char>(SEND_RESTORE_LIFE_CHEAT));
    protocol.send_message(msg_restore_life_cheat);
    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }

    //vida infinita
    std::string msg_infinite_life_cheat;
    msg_infinite_life_cheat.push_back(static_cast<char>(SEND_INFINITE_LIFE_CHEAT));
    protocol.send_message(msg_infinite_life_cheat);
    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }

    //perder carrera
    std::string msg_lose_race_cheat;
    msg_lose_race_cheat.push_back(static_cast<char>(SEND_LOSE_RACE_CHEAT));
    protocol.send_message(msg_lose_race_cheat);
    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }
    //nitro infinito
    std::string msg_infinite_nitro_cheat;
    msg_infinite_nitro_cheat.push_back(static_cast<char>(SEND_INFINITE_NITRO_CHEAT));
    protocol.send_message(msg_infinite_nitro_cheat);
    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }  

    //mensaje 4 quit
    std::string msg_quit;
    msg_quit.push_back(static_cast<char>(SEND_QUIT));
    protocol.send_message(msg_quit);
    if (const std::string response = protocol.receive_message(); !response.empty()) {
        std::cout << "[CLIENT] Received: " << response << std::endl;
    }

    
}