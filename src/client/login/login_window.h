#ifndef TALLER_TP_LOGIN_WINDOW_H
#define TALLER_TP_LOGIN_WINDOW_H

#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPainter>
#include <QPushButton>

#include <qlabel.h>

class LoginWindow final: public QWidget {
    Q_OBJECT

public:
    void player_name(QLabel*& nameLabel);
    void car_sellec(QLabel*& carLabel);
    void map_sellec(QLabel*& mapLabel);
    void effects() const;
    void start_button();
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow() override;

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
};

#endif  // TALLER_TP_LOGIN_WINDOW_H
