#ifndef RESULTS_TABLE_WIDGET_H
#define RESULTS_TABLE_WIDGET_H

#include <QTableWidget>
#include "src/common/DTO.h"

class ResultsTableWidget : public QTableWidget {
    Q_OBJECT

public:
    explicit ResultsTableWidget(QWidget* parent = nullptr);
    void displayResults(const FinalScoreList& scores);

private:
    void setupTable();
    void populateRow(int row, const CarRacingInfo& score);
};

#endif // RESULTS_TABLE_WIDGET_H