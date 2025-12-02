#ifndef TALLER_TP_LOGIN_WINDOW_H
#define TALLER_TP_LOGIN_WINDOW_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPixmap>
#include "car_selector_widget.h"
#include "../../common/constants.h"
#include <QScrollArea>
struct PlayerConfig {
    std::string playerName;
    int carId;
    std::string mapName;
};

class LoginWindow final: public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow() override;
    void setupUI();
    void setupNameSection();
    void setupCarSection();
    void setupMapSection();
    void setupGameActions();
    void setupConnections();
    void setupExitButton();
    uint8_t getLobbyAction() const;
    std::string getSelectedGameId() const;
    PlayerConfig getPlayerConfig() const;
    signals:
        void startButtonClicked();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVBoxLayout* layout;
    QPixmap backgroundImage;
    QLineEdit* nameInput;
    CarSelectorWidget* carSelector;
    QComboBox* mapSelector;
    QPushButton* createGameButton;
    QPushButton* joinGameButton;
    QLineEdit* gameIdInput;

    uint8_t lobbyAction;
    std::string selectedGameId;
    QPushButton* exitButton;

    QScrollArea* scrollArea;
    QWidget* contentWidget;
};

#endif  // TALLER_TP_LOGIN_WINDOW_H