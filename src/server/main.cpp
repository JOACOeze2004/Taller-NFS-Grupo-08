#include "server.h"
#include <iostream>
#include <cstdlib>

#define ARGC 2
#define PORT 1

int main(int const argc, char* argv[]) try {
    if (argc != ARGC) {
        std::cerr << "Usage: ./server [PORT]" << std::endl;
        return EXIT_FAILURE;
    }

    const std::string port = argv[PORT];

    Server server(port);
    server.run();

    return EXIT_SUCCESS;

} catch (const std::exception& e) {
    std::cerr << "[SERVER] Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
}
