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
    headers << tr("Lugar") << tr("Jugador") << tr("Tiempo Total");
    setHorizontalHeaderLabels(headers);
    
    StyleManager::styleResultsTable(this);

    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    verticalHeader()->setVisible(false);
}

void ResultsTableWidget::displayResults(const FinalScoreList& scores) {
    setRowCount(scores.size());

    for (size_t i = 0; i < scores.size(); ++i) {
        populateRow(i, scores[i]);
    }

    qDebug() << "Resultados finales de " << scores.size() << " jugadores cargados con éxito.";
}

void ResultsTableWidget::populateRow(int row, const playerDTO& score) {
    QTableWidgetItem* posItem = new QTableWidgetItem(QString::number(score.position));
    posItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(score.name));

    QTableWidgetItem* timeItem = new QTableWidgetItem(QString::number(score.time, 'f', 3));
    timeItem->setTextAlignment(Qt::AlignCenter);

    setItem(row, 0, posItem);
    setItem(row, 1, nameItem);
    setItem(row, 2, timeItem);
}