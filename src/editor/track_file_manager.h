#ifndef TALLER_TP_TRACK_FILE_MANAGER_H
#define TALLER_TP_TRACK_FILE_MANAGER_H

#include <QString>
#include <QWidget>
#include "race_track_data.h"

class TrackFileManager {
public:
    static bool saveTrackWithDialog(QWidget* parent, const RaceTrackData& data, const QString& currentCity);
    static bool loadTrackWithDialog(QWidget* parent, RaceTrackData& data, QString& currentCity);

private:
    static QString getServerTracksDirectory();
    static QString getDefaultSavePath(const QString& cityId, const QString& trackName);
    static bool confirmCityChange(QWidget* parent, const QString& trackCity, const QString& currentCity);
};

#endif // TALLER_TP_TRACK_FILE_MANAGER_H