#include "style_manager.h"
#include <QPropertyAnimation>

void StyleManager::styleButton(QPushButton* btn) {
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

void StyleManager::applyGlowEffect(QPushButton* btn) {
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

void StyleManager::styleInput(QLineEdit* input) {
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

void StyleManager::styleCombo(QComboBox* combo) {
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

void StyleManager::styleCarButton(QPushButton* btn, bool selected) {
    if (selected) {
        btn->setStyleSheet(
            "CarButton {"
            "   background-color: rgba(0, 60, 80, 220);"
            "   border: 3px solid #66ffff;"
            "   border-radius: 10px;"
            "}"
        );
    } else {
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

void StyleManager::applyGlowEffectToLabel(QLabel* label) {
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

void StyleManager::styleResultsTable(QTableWidget* table) {
    table->setStyleSheet(
        "QTableWidget {"
        "   background-color: rgba(0, 0, 0, 150);"
        "   color: white;"
        "   font-size: 24px;"
        "   border: 2px solid #00eaff;"
        "   border-radius: 10px;"
        "}"
        "QHeaderView::section {"
        "   background-color: rgba(0, 50, 70, 200);"
        "   color: #00eaff;"
        "   padding: 8px;"
        "   border: 1px solid #00eaff;"
        "   font-weight: bold;"
        "}"
        "QTableWidget::item {"
        "   padding: 10px 5px;"
        "}"
    );
}