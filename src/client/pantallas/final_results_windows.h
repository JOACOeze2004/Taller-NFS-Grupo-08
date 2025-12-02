#ifndef TALLER_TP_FINAL_RESULTS_WINDOWS_H
#define TALLER_TP_FINAL_RESULTS_WINDOWS_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPixmap>
#include <QLabel>
#include "results_table_widget.h"
#include "src/common/DTO.h"

class FinalResultsWindow : public QWidget {
    Q_OBJECT

public:
    explicit FinalResultsWindow(QWidget* parent = nullptr);
    void displayResults(const FinalScoreList& scores, const std::string& playerName);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QVBoxLayout* layout;
    QPixmap backgroundImage;
    ResultsTableWidget* resultsTable;
    QLabel* resultMessageLabel;

    void setupUI();
    void setupTitle();
    void setupResultMessage();
    void setupTable();
    void setupExitButton();
    void updateResultMessage(bool won);
};

#endif  // TALLER_TP_FINAL_RESULTS_WINDOWS_H