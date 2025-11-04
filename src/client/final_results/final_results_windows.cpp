#include "final_results_windows.h"
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QApplication>

void FinalResultsWindow::styleButton(QPushButton* btn) {
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

void FinalResultsWindow::applyGlowEffect(QPushButton* btn) {
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

void FinalResultsWindow::applyGlowEffectToLabel(QLabel* label) {
    auto* glow = new QGraphicsDropShadowEffect();
    glow->setBlurRadius(40);
    glow->setColor(QColor(0, 234, 255));
    glow->setOffset(0, 0);
    label->setGraphicsEffect(glow);

    auto* pulse = new QPropertyAnimation(glow, "blurRadius");
    pulse->setDuration(1500);
    pulse->setStartValue(30);
    pulse->setEndValue(60);
    pulse->setEasingCurve(QEasingCurve::InOutQuad);
    pulse->setLoopCount(-1);
    pulse->start();
}

FinalResultsWindow::FinalResultsWindow(QWidget *parent)
    : QWidget(parent),
      layout(new QVBoxLayout(this))
{
    setWindowTitle("Race Results");
    showMaximized();

    backgroundImage = QPixmap("../assets/images/fondo_login.png");

    layout->setContentsMargins(300, 120, 300, 100);
    layout->setSpacing(20);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    QLabel* title = new QLabel("🏁 RACE FINISHED 🏁");
    title->setStyleSheet("color: #00eaff; font-size: 48px; font-weight: bold;");
    layout->addWidget(title);

    applyGlowEffectToLabel(title);
}

void FinalResultsWindow::setMockResults() {
    QStringList results = {
        "1º  Luca  -  01:22.83",
        "2º  Joaco    -  01:30.51",
        "3º  Yoel     -  01:35.10"
    };

    for (const QString& r : results) {
        QLabel* line = new QLabel(r);
        line->setStyleSheet(
            "color: white; "
            "font-size: 32px; "
            "background-color: rgba(0,0,0,150); "
            "padding: 10px; "
            "border-radius: 10px;"
        );
        layout->addWidget(line);
    }

    layout->addSpacing(30);

    QPushButton* exitBtn = new QPushButton("EXIT");
    styleButton(exitBtn);
    applyGlowEffect(exitBtn);
    layout->addWidget(exitBtn);

    connect(exitBtn, &QPushButton::clicked, [](){
        QApplication::quit();
    });
}

void FinalResultsWindow::paintEvent(QPaintEvent* event) {
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