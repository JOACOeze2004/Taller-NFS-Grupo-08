#include "server.h"
#include <iostream>
#include <cstdlib>
#include "../common/constants.h"

int main(int const argc, char* argv[]) try {
    if (argc != ARGC_SERVER) {
        std::cerr << "Usage: ./server [PORT]" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Loading maps and other resources ..." << std::endl;
    World::preload_all_maps();
    std::cout << "Everything ready" << std::endl;
    
    const std::string port = argv[SERVER_PORT];
    std::srand(std::time(nullptr));
    Server server(port);
    server.run();

    return EXIT_SUCCESS;

} catch (const std::exception& e) {
    std::cerr << "[SERVER] Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
}
