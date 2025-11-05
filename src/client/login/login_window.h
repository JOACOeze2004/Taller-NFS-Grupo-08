#ifndef TALLER_TP_LOGIN_WINDOW_H
#define TALLER_TP_LOGIN_WINDOW_H

#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>
#include "../../common/constants.h"
#include <iostream> 

#include <qlabel.h>

struct PlayerConfig {
    std::string playerName;
    int carId;
    std::string mapName;
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
    QComboBox* carSelector;
    QComboBox* mapSelector;

    QPushButton *createGameButton;
    QPushButton *joinGameButton;
    QLineEdit* gameIdInput;
    
    uint8_t lobbyAction;
    std::string selectedGameId;
};

#endif  // TALLER_TP_LOGIN_WINDOW_H
