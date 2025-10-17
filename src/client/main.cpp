#include <iostream>
#include <sstream>
#include <string>
#include <exception>
#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>
#include "client.h"

#define ARGC 3
#define HOST 1
#define PORT 2

using namespace SDL2pp;

int main(int const argc, char *argv[]) try {
    if (argc != ARGC) {
        std::cerr << "Usage: ./client [HOST] [PORT]" << std::endl;
        return EXIT_FAILURE;
    }
    const std::string host = argv[HOST];
    const std::string port = argv[PORT];
    Client client(host, port);
    client.run();

	/*// Initialize SDL library
	SDL sdl(SDL_INIT_VIDEO);

	// Create main window: 640x480 dimensions, resizable, "SDL2pp demo" title
	Window window("SDL2pp demo",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			640, 480,
			SDL_WINDOW_RESIZABLE);

	// Create accelerated video renderer with default driver
	Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);

	// Clear screen
	renderer.Clear();

	// Show rendered frame
	renderer.Present();

	// 5 second delay
	SDL_Delay(5000);

	// Here all resources are automatically released and library deinitialized*/
	return 0;
} catch (std::exception& e) {
	// If case of error, print it and exit with error
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
}
