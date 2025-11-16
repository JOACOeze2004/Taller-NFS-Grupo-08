#ifndef TALLER_TP_FINAL_RESULTS_WINDOWS_H
#define TALLER_TP_FINAL_RESULTS_WINDOWS_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPixmap>
#include "results_table_widget.h"
#include "src/common/DTO.h"

class FinalResultsWindow : public QWidget {
    Q_OBJECT

public:
    explicit FinalResultsWindow(QWidget* parent = nullptr);
    void displayResults(const FinalScoreList& scores);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QVBoxLayout* layout;
    QPixmap backgroundImage;
    ResultsTableWidget* resultsTable;

    void setupUI();
    void setupTitle();
    void setupTable();
    void setupExitButton();
};

#endif  // TALLER_TP_FINAL_RESULTS_WINDOWS_H