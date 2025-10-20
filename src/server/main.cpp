#include "server.h"
#include <iostream>
#include <cstdlib>
#include "../common/constants.h"

int main(int const argc, char* argv[]) try {
    if (argc != ARGC_SERVER) {
        std::cerr << "Usage: ./server [PORT]" << std::endl;
        return EXIT_FAILURE;
    }

    const std::string port = argv[SERVER_PORT];

    Server server(port);
    server.run();

    return EXIT_SUCCESS;

} catch (const std::exception& e) {
    std::cerr << "[SERVER] Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
}
