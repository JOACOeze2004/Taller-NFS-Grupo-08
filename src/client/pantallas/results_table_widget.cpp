#include "results_table_widget.h"
#include "style_manager.h"
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QDebug>

ResultsTableWidget::ResultsTableWidget(QWidget* parent)
    : QTableWidget(parent) {
    setupTable();
}

void ResultsTableWidget::setupTable() {
    setColumnCount(3);
    
    QStringList headers;
    headers << tr("Place") << tr("Player") << tr("Total Time");
    setHorizontalHeaderLabels(headers);
    setStyleSheet(R"(
        QTableWidget {
            background-color: rgba(20, 20, 20, 200);
            border: 2px solid #00eaff;
            border-radius: 10px;
            gridline-color: #444444;
        }
        QHeaderView::section {
            background-color: rgba(0, 234, 255, 100);
            color: white;
            font-size: 16px;
            font-weight: bold;
            padding: 10px;
            border: none;
        }
        QTableWidget::item {
            padding: 10px;
        }
        QTableWidget::item:selected {
            background-color: rgba(0, 234, 255, 150);
        }
    )");

    horizontalHeader()->setStretchLastSection(true);
    setColumnWidth(0, 150);
    setColumnWidth(1, 300);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    verticalHeader()->setVisible(false);
    setShowGrid(true);
}

void ResultsTableWidget::displayResults(const FinalScoreList& scores) {
    setRowCount(scores.size());

    for (size_t i = 0; i < scores.size(); ++i) {
        populateRow(i, scores[i]);
    }

    qDebug() << "Resultados finales de " << scores.size() << " jugadores cargados con éxito.";
}

void ResultsTableWidget::populateRow(int row, const CarRacingInfo& score) {
    QTableWidgetItem* positionItem = new QTableWidgetItem();
    QString positionText;

    switch (score.position) {
        case 1:
            positionText = "🥇 1º";
            positionItem->setForeground(QBrush(QColor("#FFD700")));
            break;
        case 2:
            positionText = "🥈 2º";
            positionItem->setForeground(QBrush(QColor("#C0C0C0")));
            break;
        case 3:
            positionText = "🥉 3º";
            positionItem->setForeground(QBrush(QColor("#CD7F32")));
            break;
        default:
            positionText = QString::number(score.position) + "º";
            positionItem->setForeground(QBrush(QColor("#00eaff")));
            break;
    }

    positionItem->setText(positionText);
    positionItem->setTextAlignment(Qt::AlignCenter);
    positionItem->setFont(QFont("Arial", 16, QFont::Bold));
    setItem(row, 0, positionItem);

    QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(score.name));
    nameItem->setTextAlignment(Qt::AlignCenter);
    nameItem->setFont(QFont("Arial", 14, QFont::Bold));
    nameItem->setForeground(QBrush(QColor("#FFFFFF")));
    setItem(row, 1, nameItem);

    QTableWidgetItem* timeItem = new QTableWidgetItem();

    float total_time_seconds = score.time;

    int minutes = static_cast<int>(total_time_seconds) / 60;
    int seconds = static_cast<int>(total_time_seconds) % 60;
    float decimal_part = total_time_seconds - static_cast<int>(total_time_seconds);
    int milliseconds = static_cast<int>(decimal_part * 1000);

    QString timeStr = QString("%1:%2.%3")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
        .arg(milliseconds, 3, 10, QChar('0'));

    timeItem->setText(timeStr);
    timeItem->setTextAlignment(Qt::AlignCenter);
    timeItem->setFont(QFont("Courier New", 14, QFont::Bold));
    timeItem->setForeground(QBrush(QColor("#00FF7F")));
    setItem(row, 2, timeItem);

    QColor rowColor = (row % 2 == 0) ? QColor(40, 40, 40, 180) : QColor(60, 60, 60, 180);
    for (int col = 0; col < 3; ++col) {
        item(row, col)->setBackground(QBrush(rowColor));
    }
}