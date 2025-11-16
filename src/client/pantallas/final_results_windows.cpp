#include "final_results_windows.h"
#include "style_manager.h"
#include <QPainter>
#include <QLabel>
#include <QPushButton>
#include <QApplication>

FinalResultsWindow::FinalResultsWindow(QWidget* parent)
    : QWidget(parent),
      layout(new QVBoxLayout(this)),
      resultsTable(nullptr)
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
    setupTable();
    setupExitButton();

    setLayout(layout);
}

void FinalResultsWindow::setupTitle() {
    QLabel* title = new QLabel("🏁 RESULTADOS FINALES 🏁");
    title->setStyleSheet("color: #00eaff; font-size: 48px; font-weight: bold;");
    StyleManager::applyGlowEffectToLabel(title);
    layout->addWidget(title);
}

void FinalResultsWindow::setupTable() {
    resultsTable = new ResultsTableWidget(this);
    layout->addWidget(resultsTable);
}

void FinalResultsWindow::setupExitButton() {
    QPushButton* exitBtn = new QPushButton("SALIR");
    StyleManager::styleButton(exitBtn);
    StyleManager::applyGlowEffect(exitBtn);
    layout->addWidget(exitBtn);

    connect(exitBtn, &QPushButton::clicked, []() {
        QApplication::quit();
    });
}

void FinalResultsWindow::displayResults(const FinalScoreList& scores) {
    resultsTable->displayResults(scores);
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