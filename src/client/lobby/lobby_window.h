#ifndef TALLER_TP_LOBBY_WINDOW_H
#define TALLER_TP_LOBBY_WINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPixmap>
#include <QTimer>
#include <QPaintEvent>
#include <QProgressBar>

class LobbyWindow final: public QWidget {
    Q_OBJECT

public:
    explicit LobbyWindow(QWidget *parent = nullptr);
    ~LobbyWindow() override;

    void setGameId(const std::string& gameId);

    signals:
        void exitButtonClicked();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void styleButton(QPushButton* btn);
    void applyGlowEffect(QPushButton* btn);
    void updateLoadingText();
    void updateProgressBar();

    QVBoxLayout *layout;
    QLabel *titleLabel;
    QLabel *loadingLabel;
    QLabel *gameIdLabel;
    QProgressBar *progressBar;
    QPushButton *exitButton;
    QPixmap backgroundImage;
    QTimer *loadingTimer;
    QTimer *progressTimer;
    int dotCount;
    int progressValue;
};

#endif  // TALLER_TP_LOBBY_WINDOW_H