#include "city_selec.h"
#include "city_config.h"
#include <QFont>
#include <QFile>
#include <QDebug>
#include <QLabel>
#include <QMessageBox>

CitySelection::CitySelection(QWidget *parent)
    : QDialog(parent), selectedCity("") {

    setWindowTitle("Need for Speed - Selección de Mapa");
    setModal(true);
    setFixedSize(700, 500);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    QLabel *titleLabel = new QLabel("Selecciona tu Mapa");
    QFont titleFont;
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #2c3e50; margin-bottom: 30px;");
    mainLayout->addWidget(titleLabel);

    QVector<CityInfo> cities = CityConfig::getAllCities();
    for (const auto& city : cities) {
        createCityCard(mainLayout, city);
    }

    mainLayout->addStretch();
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
    return thumbnail.scaled(120, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void CitySelection::createCityCard(QLayout *parentLayout, const CityInfo& city) {
    QWidget *cardWidget = new QWidget();
    cardWidget->setStyleSheet(
        "QWidget {"
        "   background-color: #ecf0f1;"
        "   border: 2px solid #bdc3c7;"
        "   border-radius: 10px;"
        "}"
    );

    QHBoxLayout *cardLayout = new QHBoxLayout(cardWidget);
    cardLayout->setSpacing(20);
    cardLayout->setContentsMargins(15, 15, 15, 15);

    QPixmap cityImage = loadCityThumbnail(city.imagePath);
    if (cityImage.isNull()) {
        qWarning() << "No se pudo cargar la ciudad:" << city.displayName;
        delete cardWidget;
        return;
    }

    QLabel *imageLabel = new QLabel();
    imageLabel->setPixmap(cityImage);
    imageLabel->setFixedSize(120, 80);
    imageLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(imageLabel);

    QLabel *nameLabel = new QLabel(city.displayName);
    QFont nameFont;
    nameFont.setPointSize(16);
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setStyleSheet("color: #2c3e50;");
    cardLayout->addWidget(nameLabel, 1);

    QPushButton *selectButton = new QPushButton("Seleccionar");
    selectButton->setFixedSize(120, 40);
    selectButton->setCursor(Qt::PointingHandCursor);
    selectButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #3498db;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover { background-color: #2980b9; }"
        "QPushButton:pressed { background-color: #21618c; }"
    );

    connect(selectButton, &QPushButton::clicked, [this, city]() {
        selectedCity = city.id;
        accept();
    });

    cardLayout->addWidget(selectButton);
    parentLayout->addWidget(cardWidget);
}