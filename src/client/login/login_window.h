#ifndef TALLER_TP_LOGIN_WINDOW_H
#define TALLER_TP_LOGIN_WINDOW_H

#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include <QGridLayout>
#include <optional>
#include "../../common/constants.h"
#include <iostream>

#include <qlabel.h>

struct PlayerConfig {
    std::string playerName;
    int carId;
    std::string mapName;
};

class CarButton : public QPushButton {
    Q_OBJECT
public:
    CarButton(int carId, QWidget* parent = nullptr);
    int getCarId() const { return carId; }

private:
    int carId;
};

class LoginWindow final: public QWidget {
    Q_OBJECT

public:
    void styleButton(QPushButton* btn);
    void applyGlowEffect(QPushButton* btn);
    void styleInput(QLineEdit* input);
    void styleCombo(QComboBox* combo);
    void player_name(QLabel*& nameLabel);
    void car_sellec(QLabel*& carLabel);
    void map_sellec(QLabel*& mapLabel);
    void updateGameList(const std::vector<std::string>& gameList);

    explicit LoginWindow(QWidget *parent = nullptr);

    ~LoginWindow() override;

    uint8_t getLobbyAction() const;
    std::string getSelectedGameId() const;

    PlayerConfig getPlayerConfig() const;
    signals:
        void startButtonClicked();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPushButton *startButton;
    QVBoxLayout *layout;
    QPixmap backgroundImage;
    QLineEdit* nameInput;
    QWidget* carSelectorWidget;
    QGridLayout* carGridLayout;
    std::vector<CarButton*> carButtons;
    int selectedCarIndex;
    QComboBox* mapSelector;
    QComboBox* gameSelector;
    QLabel* gameLabel;

    QPushButton *createGameButton;
    QPushButton *joinGameButton;
    QLineEdit* gameIdInput;

    uint8_t lobbyAction;
    std::string selectedGameId;

    void selectCar(int carId);
};

#endif  // TALLER_TP_LOGIN_WINDOW_H