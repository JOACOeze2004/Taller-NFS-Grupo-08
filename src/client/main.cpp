#include <iostream>
#include <sstream>
#include <string>
#include <exception>
#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>
#include "client.h"
#include "../common/constants.h"
#include "login/login_window.h"
#include <QApplication>
#include <QWidget>

using namespace SDL2pp;

int main(int argc, char *argv[]) try {
    if (argc != ARGC) {
        std::cerr << "Usage: ./client [HOST] [PORT]" << std::endl;
        return EXIT_FAILURE;
    }
    const std::string host = argv[HOST];
    const std::string port = argv[PORT];

    const QApplication app(argc, argv);
    const auto loginWindow = new LoginWindow();
    bool startPressed = false;
    PlayerConfig playerConfig;

    QObject::connect(loginWindow, &LoginWindow::startButtonClicked, [&]() {
        playerConfig = loginWindow->getPlayerConfig();
        startPressed = true;
        loginWindow->close();
        QApplication::quit();
    });

    loginWindow->show();
    QApplication::exec();

    if (startPressed) {
        Client client(host, port);
        client.run(playerConfig);
    }

	return 0;
} catch (std::exception& e) {
	// If case of error, print it and exit with error
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
}