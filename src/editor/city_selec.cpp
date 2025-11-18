#include "city_selec.h"
#include "city_config.h"
#include <QFont>
#include <QFile>
#include <QDebug>
#include <QLabel>
#include <QMessageBox>
#include <QScreen>
#include <QApplication>
#include <QGraphicsDropShadowEffect>

CitySelection::CitySelection(QWidget *parent)
    : QDialog(parent), selectedCity("") {

    setWindowTitle("Need for Speed - Selección de Mapa");
    setModal(true);
    showMaximized();

    setStyleSheet(
        "QDialog {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #1a1a2e, stop:0.5 #16213e, stop:1 #0f3460);"
        "}"
    );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(30);
    mainLayout->setContentsMargins(60, 50, 60, 50);

    QLabel *titleLabel = new QLabel("SELECCIONA TU CIUDAD");
    QFont titleFont("Arial", 42, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "color: #00d4ff;"
        "background: transparent;"
        "padding: 30px;"
        "letter-spacing: 3px;"
    );

    QGraphicsDropShadowEffect *titleShadow = new QGraphicsDropShadowEffect();
    titleShadow->setBlurRadius(20);
    titleShadow->setColor(QColor(0, 212, 255, 180));
    titleShadow->setOffset(0, 0);
    titleLabel->setGraphicsEffect(titleShadow);

    mainLayout->addWidget(titleLabel);

    QWidget *cardsContainer = new QWidget();
    QVBoxLayout *cardsLayout = new QVBoxLayout(cardsContainer);
    cardsLayout->setSpacing(25);

    QVector<CityInfo> cities = CityConfig::getAllCities();
    for (const auto& city : cities) {
        createCityCard(cardsLayout, city);
    }

    mainLayout->addWidget(cardsContainer);
    mainLayout->addStretch();

    QLabel *footerLabel = new QLabel("Presiona ESC para salir");
    footerLabel->setAlignment(Qt::AlignCenter);
    footerLabel->setStyleSheet(
        "color: #718096;"
        "background: transparent;"
        "font-size: 12px;"
        "margin-top: 10px;"
    );
    mainLayout->addWidget(footerLabel);

    setLayout(mainLayout);
}

QPixmap CitySelection::loadCityThumbnail(const QString &imagePath) {
    if (!QFile::exists(imagePath)) {
        qCritical() << "ERROR: Imagen no encontrada:" << imagePath;
        return QPixmap();
    }
    QPixmap thumbnail;
    if (!thumbnail.load(imagePath)) {
        qCritical() << "ERROR: No se pudo cargar la imagen:" << imagePath;
        return QPixmap();
    }
    return thumbnail.scaled(250, 170, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void CitySelection::createCityCard(QLayout *parentLayout, const CityInfo& city) {
    QWidget *cardWidget = new QWidget();
    cardWidget->setFixedHeight(200);
    cardWidget->setStyleSheet(
        "QWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 rgba(30, 30, 46, 200), stop:1 rgba(40, 40, 60, 200));"
        "   border: 2px solid #00d4ff;"
        "   border-radius: 15px;"
        "}"
        "QWidget:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 rgba(40, 40, 60, 220), stop:1 rgba(50, 50, 80, 220));"
        "   border: 3px solid #00f5ff;"
        "}"
    );

    QGraphicsDropShadowEffect *cardShadow = new QGraphicsDropShadowEffect();
    cardShadow->setBlurRadius(25);
    cardShadow->setColor(QColor(0, 212, 255, 100));
    cardShadow->setOffset(0, 5);
    cardWidget->setGraphicsEffect(cardShadow);

    QHBoxLayout *cardLayout = new QHBoxLayout(cardWidget);
    cardLayout->setSpacing(40);
    cardLayout->setContentsMargins(30, 25, 30, 25);

    QPixmap cityImage = loadCityThumbnail(city.imagePath);
    if (cityImage.isNull()) {
        qWarning() << "No se pudo cargar la ciudad:" << city.displayName;
        delete cardWidget;
        return;
    }

    QLabel *imageLabel = new QLabel();
    imageLabel->setPixmap(cityImage);
    imageLabel->setFixedSize(250, 170);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setStyleSheet(
        "border: 3px solid #00d4ff;"
        "border-radius: 10px;"
        "background-color: #000;"
        "padding: 2px;"
    );
    cardLayout->addWidget(imageLabel);

    QLabel *nameLabel = new QLabel(city.displayName);
    QFont nameFont("Arial", 32, QFont::Bold);
    nameLabel->setFont(nameFont);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet(
        "color: #ffffff;"
        "background: transparent;"
        "letter-spacing: 2px;"
    );
    cardLayout->addWidget(nameLabel, 1);

    QPushButton *selectButton = new QPushButton("SELECCIONAR");
    selectButton->setFixedSize(200, 60);
    selectButton->setCursor(Qt::PointingHandCursor);
    selectButton->setStyleSheet(
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #00d4ff, stop:1 #0099cc);"
        "   color: #000000;"
        "   border: none;"
        "   border-radius: 10px;"
        "   font-weight: bold;"
        "   font-size: 18px;"
        "   letter-spacing: 2px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #00f5ff, stop:1 #00bfff);"
        "}"
        "QPushButton:pressed {"
        "   background: #008fb3;"
        "}"
    );

    connect(selectButton, &QPushButton::clicked, [this, city]() {
        selectedCity = city.id;
        accept();
    });

    cardLayout->addWidget(selectButton);
    parentLayout->addWidget(cardWidget);
}