#include "login_window.h"
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPixmap>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QApplication>
#include <QScreen>
#include <QPropertyAnimation>
#include <QKeyEvent>

void LoginWindow::player_name(QLabel*& nameLabel) {
    nameLabel = new QLabel("Nombre del jugador:");
    nameLabel->setStyleSheet("color: #00eaff; font-size: 22px; font-weight: bold;");
    nameInput = new QLineEdit();
    nameInput->setPlaceholderText("Ingresá tu nombre...");
    nameInput->setStyleSheet("QLineEdit {"
                             "   background-color: rgba(20, 20, 30, 200);"
                             "   color: white;"
                             "   border: 2px solid #00eaff;"
                             "   border-radius: 10px;"
                             "   padding: 8px;"
                             "   font-size: 20px;"
                             "}");
}
void LoginWindow::car_sellec(QLabel*& carLabel) {
    carLabel = new QLabel("Choose your vehicle:");
    carLabel->setStyleSheet("color: #00eaff; font-size: 22px; font-weight: bold;");
    carSelector = new QComboBox();
    carSelector->addItem("auto 0");
    carSelector->addItem("auto 1");
    carSelector->addItem("auto 2");
    carSelector->addItem("auto 3");
    carSelector->addItem("auto 4");
    carSelector->addItem("auto 5");
    carSelector->addItem("auto 6");

    carSelector->setStyleSheet("QComboBox {"
                               "   background-color: rgba(20, 20, 30, 200);"
                               "   color: white;"
                               "   border: 2px solid #00eaff;"
                               "   border-radius: 10px;"
                               "   padding: 6px;"
                               "   font-size: 20px;"
                               "}"
                               "QComboBox::drop-down {"
                               "   border: none;"
                               "}");
}
void LoginWindow::map_sellec(QLabel*& mapLabel) {
    mapLabel = new QLabel("Map:");
    mapLabel->setStyleSheet("color: #00eaff; font-size: 22px; font-weight: bold;");
    mapSelector = new QComboBox();
    mapSelector->addItem("Liberty City");
    mapSelector->addItem("San Andreas");
    mapSelector->addItem("Vice City");
    mapSelector->setStyleSheet("QComboBox {"
                               "   background-color: rgba(20, 20, 30, 200);"
                               "   color: white;"
                               "   border: 2px solid #00eaff;"
                               "   border-radius: 10px;"
                               "   padding: 6px;"
                               "   font-size: 20px;"
                               "}");
}
void LoginWindow::effects() const {
    const auto glowEffect = new QGraphicsDropShadowEffect();
    glowEffect->setBlurRadius(40);
    glowEffect->setColor(QColor(0, 234, 255));
    glowEffect->setOffset(0, 0);
    startButton->setGraphicsEffect(glowEffect);

    const auto pulse = new QPropertyAnimation(glowEffect, "blurRadius");
    pulse->setDuration(1500);
    pulse->setStartValue(30);
    pulse->setEndValue(60);
    pulse->setEasingCurve(QEasingCurve::InOutQuad);
    pulse->setLoopCount(-1);
    pulse->start();
}
void LoginWindow::start_button() {
    startButton = new QPushButton("START RACE", this);
    startButton->setStyleSheet("QPushButton {"
                               "   font-size: 32px;"
                               "   font-weight: bold;"
                               "   color: #00eaff;"
                               "   background-color: rgba(10, 10, 20, 180);"
                               "   border: 2px solid #00eaff;"
                               "   border-radius: 18px;"
                               "   padding: 10px 24px;"
                               "}"
                               "QPushButton:hover {"
                               "   background-color: rgba(0, 60, 80, 200);"
                               "   color: white;"
                               "   border: 2px solid #66ffff;"
                               "}"
                               "QPushButton:pressed {"
                               "   background-color: rgba(0, 120, 150, 255);"
                               "   color: black;"
                               "}");
}

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent),
      startButton(nullptr),
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

    QLabel* nameLabel;
    player_name(nameLabel);

    QLabel* carLabel;
    car_sellec(carLabel);

    QLabel* mapLabel;
    map_sellec(mapLabel);

    start_button();

    effects();

    layout = new QVBoxLayout(this);
    layout->setContentsMargins(400, 100, 400, 100);
    layout->setSpacing(30);
    layout->setAlignment(Qt::AlignTop);

    layout->addWidget(nameLabel);
    layout->addWidget(nameInput);
    layout->addWidget(carLabel);
    layout->addWidget(carSelector);
    layout->addWidget(mapLabel);
    layout->addWidget(mapSelector);
    layout->addSpacing(40);

    createGameButton = new QPushButton("CREATE GAME");
    createGameButton->setStyleSheet("QPushButton {"
                                "   font-size: 28px;"
                                "   font-weight: bold;"
                                "   color: #00eaff;"
                                "   background-color: rgba(10, 10, 20, 180);"
                                "   border: 2px solid #00eaff;"
                                "   border-radius: 12px;"
                                "   padding: 15px 30px;"
                                "}");
    layout->addWidget(createGameButton, 0, Qt::AlignHCenter);

    gameIdInput = new QLineEdit();
    gameIdInput->setPlaceholderText("Enter the game ID...");
    gameIdInput->setStyleSheet("QLineEdit {"
                               "   background-color: rgba(20, 20, 30, 200);"
                               "   color: white;"
                               "   border: 2px solid #00eaff;"
                               "   border-radius: 10px;"
                               "   padding: 8px;"
                               "   font-size: 20px;"
                               "}");
    layout->addWidget(gameIdInput);
    
    joinGameButton = new QPushButton("JOIN GAME");
    joinGameButton->setStyleSheet("QPushButton {"
                                "   font-size: 28px;"
                                "   font-weight: bold;"
                                "   color: #00eaff;"
                                "   background-color: rgba(10, 10, 20, 180);"
                                "   border: 2px solid #00eaff;"
                                "   border-radius: 12px;"
                                "   padding: 15px 30px;"
                                "}");

    layout->addWidget(joinGameButton, 0, Qt::AlignHCenter);

    layout->addWidget(startButton, 0, Qt::AlignHCenter);

    setLayout(layout);

    connect(startButton, &QPushButton::clicked, this, [this]() {
        this->lobbyAction = SEND_CREATE_GAME;
        this->selectedGameId = "";
        emit startButtonClicked();
    });
    
    connect(createGameButton, &QPushButton::clicked, this, [this]() {
        this->lobbyAction = SEND_CREATE_GAME;
        this->selectedGameId = "";
        emit startButtonClicked();
    });

    connect(joinGameButton, &QPushButton::clicked, this, [this]() {
        std::string gameId = gameIdInput->text().toStdString();
        
        if (gameId.empty()) {
            std::cout << "[CLIENT] ERROR: Debes ingresar un ID de partida" << std::endl;
            return;
        }
        this->lobbyAction = SEND_JOIN_GAME;
        this->selectedGameId = gameId;
        emit startButtonClicked();
    });
}

LoginWindow::~LoginWindow() = default;

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
        config.playerName = "Player";
    }
    config.carId = carSelector->currentIndex();
    config.mapName = mapSelector->currentText().toStdString();

    return config;
}