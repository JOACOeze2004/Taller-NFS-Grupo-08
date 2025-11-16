#include "login_window.h"
#include "style_manager.h"
#include <QLabel>
#include <QPainter>
#include <iostream>
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent),
      layout(nullptr),
      nameInput(nullptr),
      carSelector(nullptr),
      mapSelector(nullptr),
      gameIdInput(nullptr),
      lobbyAction(SEND_CREATE_GAME),
      selectedGameId("")
{
    setWindowTitle("Need for Speed - Login");
    showMaximized();
    backgroundImage = QPixmap("../assets/images/fondo_login.png");

    setupUI();
    setupConnections();
}

LoginWindow::~LoginWindow() = default;

void LoginWindow::setupUI() {
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(300, 50, 300, 50);
    layout->setSpacing(20);
    layout->setAlignment(Qt::AlignTop);

    setupNameSection();
    setupCarSection();
    setupMapSection();
    setupGameActions();

    setLayout(layout);
}

void LoginWindow::setupNameSection() {
    QLabel* nameLabel = new QLabel("Nombre del jugador:");
    nameLabel->setStyleSheet("color: #00eaff; font-size: 22px; font-weight: bold;");

    nameInput = new QLineEdit();
    nameInput->setPlaceholderText("Ingresá tu nombre...");
    StyleManager::styleInput(nameInput);

    layout->addWidget(nameLabel);
    layout->addWidget(nameInput);
}

void LoginWindow::setupCarSection() {
    QLabel* carLabel = new QLabel("Choose your vehicle:");
    carLabel->setStyleSheet("color: #00eaff; font-size: 22px; font-weight: bold;");

    carSelector = new CarSelectorWidget(this);

    layout->addWidget(carLabel);
    layout->addWidget(carSelector);
}

void LoginWindow::setupMapSection() {
    QLabel* mapLabel = new QLabel("Map:");
    mapLabel->setStyleSheet("color: #00eaff; font-size: 22px; font-weight: bold;");

    mapSelector = new QComboBox();
    mapSelector->addItem("Liberty City");
    mapSelector->addItem("San Andreas");
    mapSelector->addItem("Vice City");
    StyleManager::styleCombo(mapSelector);

    layout->addWidget(mapLabel);
    layout->addWidget(mapSelector);
}

void LoginWindow::setupGameActions() {
    layout->addSpacing(40);

    createGameButton = new QPushButton("CREATE GAME");
    StyleManager::styleButton(createGameButton);
    StyleManager::applyGlowEffect(createGameButton);
    layout->addWidget(createGameButton, 0, Qt::AlignHCenter);

    gameIdInput = new QLineEdit();
    gameIdInput->setPlaceholderText("Enter the game ID...");
    StyleManager::styleInput(gameIdInput);
    layout->addWidget(gameIdInput);

    joinGameButton = new QPushButton("JOIN GAME");
    StyleManager::styleButton(joinGameButton);
    StyleManager::applyGlowEffect(joinGameButton);
    layout->addWidget(joinGameButton, 0, Qt::AlignHCenter);
}

void LoginWindow::setupConnections() {
    connect(createGameButton, &QPushButton::clicked, this, [this]() {
        std::string playerName = nameInput->text().toStdString();

        if (playerName.empty()) {
            QMessageBox::warning(this, "Nombre requerido",
                "Por favor, ingresa tu nombre antes de crear una partida.");
            return;
        }
        this->lobbyAction = SEND_CREATE_GAME;
        this->selectedGameId = "";
        emit startButtonClicked();
    });

    connect(joinGameButton, &QPushButton::clicked, this, [this]() {
        std::string gameId = gameIdInput->text().toStdString();
        std::string playerName = nameInput->text().toStdString();

        if (playerName.empty()) {
            QMessageBox::warning(this, "Nombre requerido",
                "Por favor, ingresa tu nombre antes de unirte a una partida.");
            return;
        }
        if (gameId.empty()) {
            std::cout << "[CLIENT] ERROR: Debes ingresar un ID de partida" << std::endl;
            return;
        }
        this->lobbyAction = SEND_JOIN_GAME;
        this->selectedGameId = gameId;
        emit startButtonClicked();
    });
}

void LoginWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    if (!backgroundImage.isNull()) {
        QPixmap scaled = backgroundImage.scaled(
            this->size(),
            Qt::KeepAspectRatioByExpanding,
            Qt::SmoothTransformation
        );

        int x = (this->width() - scaled.width()) / 2;
        int y = (this->height() - scaled.height()) / 2;

        painter.drawPixmap(x, y, scaled);
    }

    QWidget::paintEvent(event);
}

uint8_t LoginWindow::getLobbyAction() const {
    return lobbyAction;
}

std::string LoginWindow::getSelectedGameId() const {
    return selectedGameId;
}

PlayerConfig LoginWindow::getPlayerConfig() const {
    PlayerConfig config;
    config.playerName = nameInput->text().toStdString();
    if (config.playerName.empty()) {
        throw std::runtime_error("Player name is required");
    }
    config.carId = carSelector->getSelectedCarId();
    config.mapName = mapSelector->currentText().toStdString();
    return config;
}