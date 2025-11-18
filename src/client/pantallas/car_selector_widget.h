#ifndef CAR_SELECTOR_WIDGET_H
#define CAR_SELECTOR_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <vector>

class CarButton : public QPushButton {
    Q_OBJECT
public:
    CarButton(int carId, QWidget* parent = nullptr);
    int getCarId() const { return carId; }

private:
    int carId;
};

class CarSelectorWidget : public QWidget {
    Q_OBJECT

public:
    explicit CarSelectorWidget(QWidget* parent = nullptr);
    int getSelectedCarId() const { return selectedCarId; }

    signals:
        void carSelected(int carId);

private slots:
    void selectCar(int carId);

private:
    QGridLayout* gridLayout;
    std::vector<CarButton*> carButtons;
    int selectedCarId;

    void setupCarButtons();
};

#endif // CAR_SELECTOR_WIDGET_H