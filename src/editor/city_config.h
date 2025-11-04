#ifndef TALLER_TP_CITY_CONFIG_H
#define TALLER_TP_CITY_CONFIG_H

#include <QString>
#include <QVector>

struct CityInfo {
    QString id;
    QString displayName;
    QString imagePath;
};

class CityConfig {
public:
    static QVector<CityInfo> getAllCities();
    static QString getAssetsBasePath();
    static QString getDisplayName(const QString& cityId);

private:
    static const QString ASSETS_BASE_PATH;
};

#endif // TALLER_TP_CITY_CONFIG_H