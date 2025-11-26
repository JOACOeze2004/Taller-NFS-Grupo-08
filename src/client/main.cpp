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
#include "pantallas/final_results_windows.h"
#include "pantallas/login_window.h"

#include "client.h"
#include "audio_manager.h"

using namespace SDL2pp;

int main(int argc, char *argv[]) try {
    if (argc != ARGC) {
        std::cerr << "Usage: ./client [HOST] [PORT]" << std::endl;
        return EXIT_FAILURE;
    }
    const std::string host = argv[HOST];
    const std::string port = argv[PORT];

    AudioManager audioManager;
    if (!audioManager.initialize()) {
        std::cerr << "Error al inicializar AudioManager" << std::endl;
    }
    audioManager.changeState(GameState::LOBBY);

    QApplication app(argc, argv);
    auto loginWindow = new LoginWindow();
    bool startPressed = false;
    PlayerConfig playerConfig;
    Client* client = nullptr;

    QObject::connect(loginWindow, &LoginWindow::startButtonClicked, [&]() {
        playerConfig = loginWindow->getPlayerConfig();
        try {
            client = new Client(host, port, &audioManager);
            client->send_config(playerConfig, loginWindow->getLobbyAction(), loginWindow->getSelectedGameId());
            loginWindow->close();
            
            audioManager.changeState(GameState::PLAYING);
            
            client->run();
            startPressed = true;
            QApplication::quit();
        } catch (const std::exception& e) {
            QMessageBox::warning(loginWindow, "Error de conexión", e.what());
        }

    });

    loginWindow->show();
    QApplication::exec();

    if (startPressed && client) {
        audioManager.changeState(GameState::RESULTS);
        
        auto resultsWindow = new FinalResultsWindow();

        if (client->has_final_results()) {
            FinalScoreList results = client->get_final_results();
            resultsWindow->displayResults(results);
        } else {
            resultsWindow->displayResults(FinalScoreList());
        }

        resultsWindow->show();
        delete client;
        return app.exec();
    }

    if (client) {
        delete client;
    }

    return 0;
} catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}