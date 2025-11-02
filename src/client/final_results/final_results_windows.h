#ifndef TALLER_TP_FINAL_RESULTS_WINDOWS_H
#define TALLER_TP_FINAL_RESULTS_WINDOWS_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>

class FinalResultsWindow : public QWidget {
    Q_OBJECT

public:
    explicit FinalResultsWindow(QWidget *parent = nullptr);

    void setMockResults();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void styleButton(QPushButton* btn);
    void applyGlowEffect(QPushButton* btn);
    void applyGlowEffectToLabel(QLabel* label);

    QVBoxLayout* layout;
    QPixmap backgroundImage;
};
#endif  // TALLER_TP_FINAL_RESULTS_WINDOWS_H