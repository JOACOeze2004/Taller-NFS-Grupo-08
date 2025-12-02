#include "car_selector_widget.h"
#include "style_manager.h"
#include <QPixmap>
#include <iostream>
#include <QSizePolicy>

CarButton::CarButton(int carId, QWidget* parent)
    : QPushButton(parent), carId(carId) {

    QString imagePath = QString("../assets/images/auto_%1.png").arg(carId);
    QPixmap carPixmap(imagePath);

    if (!carPixmap.isNull()) {
        setIcon(QIcon(carPixmap));
        setIconSize(QSize(100, 75));
    } else {
        setText(QString("Auto %1").arg(carId));
    }
    setMinimumSize(80, 60);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setCheckable(true);
}

CarSelectorWidget::CarSelectorWidget(QWidget* parent)
    : QWidget(parent), selectedCarId(0) {
    
    gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(15);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    
    setupCarButtons();
    setLayout(gridLayout);
    
    selectCar(0);
}

void CarSelectorWidget::setupCarButtons() {
    for (int i = 0; i < 7; i++) {
        CarButton* carBtn = new CarButton(i, this);
        carButtons.push_back(carBtn);

        int row = i / 4;
        int col = i % 4;
        gridLayout->addWidget(carBtn, row, col);

        connect(carBtn, &QPushButton::clicked, this, [this, i]() {
            selectCar(i);
        });
    }
}

void CarSelectorWidget::selectCar(int carId) {
    selectedCarId = carId;

    for (auto* btn : carButtons) {
        bool isSelected = (btn->getCarId() == carId);
        btn->setChecked(isSelected);
        StyleManager::styleCarButton(btn, isSelected);
    }

    emit carSelected(carId);
}