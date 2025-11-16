#ifndef STYLE_MANAGER_H
#define STYLE_MANAGER_H

#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QTableWidget>
#include <QGraphicsDropShadowEffect>

class StyleManager {
public:
    static void styleButton(QPushButton* btn);
    static void applyGlowEffect(QPushButton* btn);
    static void applyGlowEffectToLabel(QLabel* label);
    static void styleInput(QLineEdit* input);
    static void styleCombo(QComboBox* combo);
    static void styleCarButton(QPushButton* btn, bool selected);
    static void styleResultsTable(QTableWidget* table);

private:
    StyleManager() = default;
};

#endif // STYLE_MANAGER_H