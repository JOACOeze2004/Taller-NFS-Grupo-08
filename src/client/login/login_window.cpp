#include "login_window.h"
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
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

CarButton::CarButton(int carId, QWidget* parent)
    : QPushButton(parent), carId(carId) {

    QString imagePath = QString("../assets/images/auto_%1.png").arg(carId);
    QPixmap carPixmap(imagePath);

    if (!carPixmap.isNull()) {
        setIcon(QIcon(carPixmap));
        setIconSize(QSize(120, 90));
    } else {
        setText(QString("Auto %1").arg(carId));
        std::cout << "[CLIENT] WARNING: No se pudo cargar: "
                  << imagePath.toStdString() << std::endl;
    }

    setFixedSize(140, 110);
    setCheckable(true);
}

void LoginWindow::styleButton(QPushButton* btn) {
    btn->setStyleSheet(
        "QPushButton {"
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
        "}"
    );
}

void LoginWindow::applyGlowEffect(QPushButton* btn) {
    auto* glow = new QGraphicsDropShadowEffect();
    glow->setBlurRadius(40);
    glow->setColor(QColor(0, 234, 255));
    glow->setOffset(0, 0);
    btn->setGraphicsEffect(glow);

    auto* pulse = new QPropertyAnimation(glow, "blurRadius");
    pulse->setDuration(1500);
    pulse->setStartValue(30);
    pulse->setEndValue(60);
    pulse->setEasingCurve(QEasingCurve::InOutQuad);
    pulse->setLoopCount(-1);
    pulse->start();
}

void LoginWindow::styleInput(QLineEdit* input) {
    input->setStyleSheet(
        "QLineEdit {"
        "   background-color: rgba(20, 20, 30, 200);"
        "   color: white;"
        "   border: 2px solid #00eaff;"
        "   border-radius: 10px;"
        "   padding: 8px;"
        "   font-size: 20px;"
        "}"
    );
}

void LoginWindow::styleCombo(QComboBox* combo) {
    combo->setStyleSheet(
        "QComboBox {"
        "   background-color: rgba(20, 20, 30, 200);"
        "   color: grey;"
        "   border: 2px solid #00eaff;"
        "   border-radius: 10px;"
        "   padding: 6px;"
        "   font-size: 20px;"
        "}"
        "QComboBox::drop-down { border: none; }"
    );
}


void LoginWindow::player_name(QLabel*& nameLabel) {
    nameLabel = new QLabel("Nombre del jugador:");
    nameLabel->setStyleSheet("color: #00eaff; font-size: 22px; font-weight: bold;");
    nameInput = new QLineEdit();
    nameInput->setPlaceholderText("Ingresá tu nombre...");
    styleInput(nameInput);
}

void LoginWindow::selectCar(int carId) {
    selectedCarIndex = carId;

    for (auto* btn : carButtons) {
        if (btn->getCarId() == carId) {
            btn->setChecked(true);
            btn->setStyleSheet(
                "CarButton {"
                "   background-color: rgba(0, 60, 80, 220);"
                "   border: 3px solid #66ffff;"
                "   border-radius: 10px;"
                "}"
            );
        } else {
            btn->setChecked(false);
            btn->setStyleSheet(
                "CarButton {"
                "   background-color: rgba(20, 20, 30, 200);"
                "   border: 2px solid #00eaff;"
                "   border-radius: 10px;"
                "}"
                "CarButton:hover {"
                "   background-color: rgba(0, 40, 60, 180);"
                "   border: 2px solid #66ffff;"
                "}"
            );
        }
    }
}

void LoginWindow::car_sellec(QLabel*& carLabel) {
    carLabel = new QLabel("Choose your vehicle:");
    carLabel->setStyleSheet("color: #00eaff; font-size: 22px; font-weight: bold;");

    carSelectorWidget = new QWidget();
    carGridLayout = new QGridLayout(carSelectorWidget);
    carGridLayout->setSpacing(15);
    carGridLayout->setContentsMargins(0, 0, 0, 0);

    for (int i = 0; i < 7; i++) {
        CarButton* carBtn = new CarButton(i, carSelectorWidget);
        carButtons.push_back(carBtn);

        int row = i / 4;
        int col = i % 4;
        carGridLayout->addWidget(carBtn, row, col);

        connect(carBtn, &QPushButton::clicked, this, [this, i]() {
            selectCar(i);
        });
    }

    carSelectorWidget->setLayout(carGridLayout);
    selectedCarIndex = 0;
    selectCar(0);
}

void LoginWindow::map_sellec(QLabel*& mapLabel) {
    mapLabel = new QLabel("Map:");
    mapLabel->setStyleSheet("color: #00eaff; font-size: 22px; font-weight: bold;");
    mapSelector = new QComboBox();
    mapSelector->addItem("Liberty City");
    mapSelector->addItem("San Andreas");
    mapSelector->addItem("Vice City");
    styleCombo(mapSelector);
}

void LoginWindow::updateGameList(const std::vector<std::string>& gameList) {
    gameSelector->clear();

    if (gameList.empty()){
        return;
    }

    for (const auto& game : gameList) {
        gameSelector->addItem(QString::fromStdString(game));
    }
}


LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent),
      startButton(nullptr),
      layout(nullptr),
      nameInput(nullptr),
      carSelectorWidget(nullptr),
      carGridLayout(nullptr),
      selectedCarIndex(0),
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

    createGameButton = new QPushButton("CREATE GAME");
    joinGameButton = new QPushButton("JOIN GAME");

    styleButton(createGameButton);
    styleButton(joinGameButton);

    applyGlowEffect(createGameButton);
    applyGlowEffect(joinGameButton);

    layout = new QVBoxLayout(this);
    layout->setContentsMargins(300, 50, 300, 50);
    layout->setSpacing(20);
    layout->setAlignment(Qt::AlignTop);

    layout->addWidget(nameLabel);
    layout->addWidget(nameInput);
    layout->addWidget(carLabel);
    layout->addWidget(carSelectorWidget);
    layout->addWidget(mapLabel);
    layout->addWidget(mapSelector);
    layout->addSpacing(40);
    layout->addWidget(createGameButton, 0, Qt::AlignHCenter);

    gameIdInput = new QLineEdit();
    gameIdInput->setPlaceholderText("Enter the game ID...");
    styleInput(gameIdInput);
    layout->addWidget(gameIdInput);

    layout->addWidget(joinGameButton, 0, Qt::AlignHCenter);

    setLayout(layout);


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
    config.carId = selectedCarIndex;
    config.mapName = mapSelector->currentText().toStdString();

    return config;
}