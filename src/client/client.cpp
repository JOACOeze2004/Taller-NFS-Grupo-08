#include "client.h"
#include "client_protocol.h"
#include "../common/constants.h"
#include "client_receiver.h"


Client::Client(const std::string& host, const std::string& port)
    : host(host), port(port), client_socket(host.c_str(), port.c_str()) {}

void Client::run() {
    std::cout << "[CLIENT] Connected to " << host << ":" << port << std::endl;

    ClientProtocol protocol(client_socket);
    
    ClientReceiver receiver(protocol); //Supongo q esta mal q esto este aca y q hagamos start.
    receiver.start();

    bool running = true;
    std::string line;
    CarDTO state;

    while (running) {
        while (receiver.try_pop_car_state(state)) {
            std::cout << "[CLIENT] Car x:" << state.x
                      << " y:" << state.y
                      << " vel:" << state.velocity
                      << " ang:" << state.angle << std::endl;
        }
        if (std::getline(std::cin, line)) {
            if (!line.empty() && line[0] == SERVER_CLOSE) {
                running = false;
                break;
            }
            uint8_t cmd = 0;
            switch (line[0]) {
                case 'w': cmd = SEND_ACCELERATE; break;
                case 'a': cmd = SEND_ROTATE_LEFT; break;
                case 'd': cmd = SEND_ROTATE_RIGHT; break;
                case 's': cmd = SEND_BRAKE; break;
                default: continue;
            }
            protocol.send_byte(cmd);
        }
    }

    receiver.stop();
    receiver.join();
    protocol.close();
}