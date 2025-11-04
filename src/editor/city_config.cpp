#include "city_config.h"
#include <QCoreApplication>

const QString CityConfig::ASSETS_BASE_PATH = "/../assets/need-for-speed/cities/";

QVector<CityInfo> CityConfig::getAllCities() {
    QString basePath = getAssetsBasePath();

    return {
            {
                "liberty_city",
                "Liberty City",
                basePath + "Game Boy _ GBC - Grand Theft Auto - Backgrounds - Liberty City.png"
            },
            {
                "san_andreas",
                "San Andreas",
                basePath + "Game Boy _ GBC - Grand Theft Auto - Backgrounds - San Andreas.png"
            },
            {
                "vice_city",
                "Vice City",
                basePath + "Game Boy _ GBC - Grand Theft Auto - Backgrounds - Vice City.png"
            }
    };
}

QString CityConfig::getAssetsBasePath() {
    return QCoreApplication::applicationDirPath() + ASSETS_BASE_PATH;
}

QString CityConfig::getDisplayName(const QString& cityId) {
    if (cityId == "liberty_city") return "Liberty City";
    if (cityId == "san_andreas") return "San Andreas";
    if (cityId == "vice_city") return "Vice City";
    return cityId;
}