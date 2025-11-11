#include <QApplication>
#include <QMessageBox>
#include <QWidget>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#include "../common/constants.h"
#include "final_results/final_results_windows.h"
#include "lobby/lobby_window.h"
#include "login/login_window.h"

#include "client.h"

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
    const auto lobbyWindow = new LobbyWindow();
    bool startPressed = false;
    PlayerConfig playerConfig;

    QObject::connect(loginWindow, &LoginWindow::startButtonClicked, [&]() {
        playerConfig = loginWindow->getPlayerConfig();
        try {
            Client client(host, port);
            client.send_config(playerConfig,loginWindow->getLobbyAction(), loginWindow->getSelectedGameId());
            loginWindow->close();
            if (loginWindow->getLobbyAction() == SEND_CREATE_GAME) {
                lobbyWindow->show();
                client.wait_lobby();
                lobbyWindow->close();
            }
            client.run();
            startPressed = true;
            QApplication::quit();
        } catch (const std::exception& e) {
            QMessageBox::warning(loginWindow, "No existe esa partida", e.what());
        }

    });

    loginWindow->show();
    QApplication::exec();

    if (startPressed) {

        auto results = new FinalResultsWindow();
        results->setMockResults();
        results->show();

        return app.exec();
    }

	return 0;
} catch (std::exception& e) {
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
}