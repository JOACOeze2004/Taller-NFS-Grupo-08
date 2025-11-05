#ifndef TALLER_TP_CITY_SELEC_H
#define TALLER_TP_CITY_SELEC_H

#include <QDialog>
#include <QPushButton>
#include <QString>
#include <QHBoxLayout>
#include <QPixmap>

struct CityInfo;

class CitySelection : public QDialog {
    Q_OBJECT

public:
    explicit CitySelection(QWidget *parent = nullptr);
    ~CitySelection() override = default;

    QString getSelectedCity() const { return selectedCity; }

private:
    QString selectedCity;

    void createCityCard(QLayout* parentLayout, const CityInfo& city);
    QPixmap loadCityThumbnail(const QString &imagePath);
};

#endif  // TALLER_TP_CITY_SELEC_H
