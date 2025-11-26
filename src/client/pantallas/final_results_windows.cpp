#include "final_results_windows.h"
#include "style_manager.h"
#include <QPainter>
#include <QLabel>
#include <QPushButton>
#include <QApplication>

FinalResultsWindow::FinalResultsWindow(QWidget* parent)
    : QWidget(parent),
      layout(new QVBoxLayout(this)),
      resultsTable(nullptr),
      resultMessageLabel(nullptr)
{
    setWindowTitle("Race Results");
    showMaximized();

    backgroundImage = QPixmap("../assets/images/fondo_login.png");

    setupUI();
}

void FinalResultsWindow::setupUI() {
    layout->setContentsMargins(100, 80, 100, 80);
    layout->setSpacing(20);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    setupTitle();
    setupResultMessage();
    setupTable();
    setupExitButton();

    setLayout(layout);
}

void FinalResultsWindow::setupTitle() {
    QLabel* title = new QLabel("🏁 FINAL RESULTS 🏁");
    title->setStyleSheet("color: #00eaff; font-size: 48px; font-weight: bold;");
    StyleManager::applyGlowEffectToLabel(title);
    layout->addWidget(title);
}

void FinalResultsWindow::setupResultMessage() {
    resultMessageLabel = new QLabel("");
    resultMessageLabel->setAlignment(Qt::AlignCenter);
    resultMessageLabel->setStyleSheet("font-size: 56px; font-weight: bold; margin: 20px;");
    layout->addWidget(resultMessageLabel);
}

void FinalResultsWindow::setupTable() {
    resultsTable = new ResultsTableWidget(this);
    layout->addWidget(resultsTable);
}

void FinalResultsWindow::setupExitButton() {
    QPushButton* exitBtn = new QPushButton("EXIT");
    StyleManager::styleButton(exitBtn);
    StyleManager::applyGlowEffect(exitBtn);
    layout->addWidget(exitBtn);

    connect(exitBtn, &QPushButton::clicked, []() {
        QApplication::quit();
    });
}

void FinalResultsWindow::displayResults(const FinalScoreList& scores, const std::string& playerName) {
    resultsTable->displayResults(scores);
    bool won = false;
    if (!scores.empty() && scores[0].name == playerName) {
        won = true;
    }
    updateResultMessage(won);
}

void FinalResultsWindow::updateResultMessage(bool won) {
    if (won) {
        resultMessageLabel->setText("🎉 ¡GANASTE! 🎉");
        resultMessageLabel->setStyleSheet(
            "color: #FFD700; "
            "font-size: 56px; "
            "font-weight: bold; "
            "margin: 20px;"
        );
        StyleManager::applyGlowEffectToLabel(resultMessageLabel);
    } else {
        resultMessageLabel->setText("😔 PERDISTE 😔");
        resultMessageLabel->setStyleSheet(
            "color: #FF4444; "
            "font-size: 56px; "
            "font-weight: bold; "
            "margin: 20px;"
        );
        StyleManager::applyGlowEffectToLabel(resultMessageLabel);
    }
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