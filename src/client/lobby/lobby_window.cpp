#include "lobby_window.h"
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>

LobbyWindow::LobbyWindow(QWidget *parent)
    : QWidget(parent),
      layout(nullptr),
      titleLabel(nullptr),
      loadingLabel(nullptr),
      gameIdLabel(nullptr),
      progressBar(nullptr),
      exitButton(nullptr),
      loadingTimer(nullptr),
      progressTimer(nullptr),
      dotCount(0),
      progressValue(0)
{
    setWindowTitle("Need for Speed - Lobby");
    showMaximized();
    backgroundImage = QPixmap("../assets/images/fondo_login.png");

    layout = new QVBoxLayout(this);
    layout->setContentsMargins(300, 150, 300, 150);
    layout->setSpacing(40);
    layout->setAlignment(Qt::AlignCenter);

    titleLabel = new QLabel("LOBBY");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 64px;"
        "   font-weight: bold;"
        "   color: #00eaff;"
        "   background-color: transparent;"
        "}"
    );

    auto* titleGlow = new QGraphicsDropShadowEffect();
    titleGlow->setBlurRadius(50);
    titleGlow->setColor(QColor(0, 234, 255));
    titleGlow->setOffset(0, 0);
    titleLabel->setGraphicsEffect(titleGlow);

    gameIdLabel = new QLabel("");
    gameIdLabel->setAlignment(Qt::AlignCenter);
    gameIdLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 24px;"
        "   font-weight: bold;"
        "   color: #00eaff;"
        "   background-color: transparent;"
        "   padding: 10px;"
        "}"
    );

    progressBar = new QProgressBar();
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);
    progressBar->setValue(30);
    progressBar->setTextVisible(false);
    progressBar->setFixedHeight(25);

    progressBar->setStyleSheet(
        "QProgressBar {"
        "   background-color: rgba(10, 10, 20, 200);"
        "   border: 2px solid #00eaff;"
        "   border-radius: 12px;"
        "   text-align: center;"
        "}"
        "QProgressBar::chunk {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #00a0cc, stop:0.5 #00eaff, stop:1 #00a0cc);"
        "   border-radius: 10px;"
        "   margin: 1px;"
        "}"
    );

    auto* progressGlow = new QGraphicsDropShadowEffect();
    progressGlow->setBlurRadius(20);
    progressGlow->setColor(QColor(0, 234, 255, 150));
    progressGlow->setOffset(0, 0);
    progressBar->setGraphicsEffect(progressGlow);

    loadingLabel = new QLabel("Esperando más jugadores");
    loadingLabel->setAlignment(Qt::AlignCenter);
    loadingLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 36px;"
        "   font-weight: bold;"
        "   color: white;"
        "   background-color: transparent;"
        "}"
    );

    auto* loadingGlow = new QGraphicsDropShadowEffect();
    loadingGlow->setBlurRadius(30);
    loadingGlow->setColor(QColor(255, 255, 255, 150));
    loadingGlow->setOffset(0, 0);
    loadingLabel->setGraphicsEffect(loadingGlow);

    exitButton = new QPushButton("SALIR");
    styleButton(exitButton);
    applyGlowEffect(exitButton);

    layout->addWidget(titleLabel);
    layout->addSpacing(30);
    layout->addWidget(gameIdLabel);
    layout->addWidget(progressBar);
    layout->addSpacing(30);
    layout->addWidget(loadingLabel);
    layout->addSpacing(80);
    layout->addWidget(exitButton, 0, Qt::AlignHCenter);

    setLayout(layout);

    loadingTimer = new QTimer(this);
    connect(loadingTimer, &QTimer::timeout, this, &LobbyWindow::updateLoadingText);
    loadingTimer->start(500);

    progressTimer = new QTimer(this);
    connect(progressTimer, &QTimer::timeout, this, &LobbyWindow::updateProgressBar);
    progressTimer->start(100);

    connect(exitButton, &QPushButton::clicked, this, [this]() {
        emit exitButtonClicked();
    });
}

LobbyWindow::~LobbyWindow() {
    if (loadingTimer) {
        loadingTimer->stop();
    }
    if (progressTimer) {
        progressTimer->stop();
    }
}

void LobbyWindow::styleButton(QPushButton* btn) {
    btn->setStyleSheet(
        "QPushButton {"
        "   font-size: 32px;"
        "   font-weight: bold;"
        "   color: #ff4444;"
        "   background-color: rgba(10, 10, 20, 180);"
        "   border: 2px solid #ff4444;"
        "   border-radius: 18px;"
        "   padding: 10px 24px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(80, 20, 20, 200);"
        "   color: white;"
        "   border: 2px solid #ff6666;"
        "}"
        "QPushButton:pressed {"
        "   background-color: rgba(150, 30, 30, 255);"
        "   color: black;"
        "}"
    );
}

void LobbyWindow::applyGlowEffect(QPushButton* btn) {
    auto* glow = new QGraphicsDropShadowEffect();
    glow->setBlurRadius(40);
    glow->setColor(QColor(255, 68, 68));
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

void LobbyWindow::updateLoadingText() {
    dotCount = (dotCount + 1) % 4;
    QString dots = QString(".").repeated(dotCount);
    loadingLabel->setText("Esperando más jugadores" + dots);
}

void LobbyWindow::updateProgressBar() {
    progressValue += 1;
    if (progressValue > 95) {
        progressValue = 30;
    }
    progressBar->setValue(progressValue);
}

void LobbyWindow::setGameId(const std::string& gameId) {
    if (!gameId.empty()) {
        gameIdLabel->setText(QString("Game ID: %1").arg(QString::fromStdString(gameId)));
    } else {
        gameIdLabel->setText("");
    }
}

void LobbyWindow::paintEvent(QPaintEvent *event) {
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
