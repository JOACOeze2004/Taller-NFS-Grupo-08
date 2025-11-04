#ifndef TALLER_TP_TRACK_SERIALIZER_H
#define TALLER_TP_TRACK_SERIALIZER_H

#include <QString>
#include "race_track_data.h"

class TrackSerializer {
public:
    static bool saveToFile(const RaceTrackData& data, const QString& filePath);
    static bool loadFromFile(const QString& filePath, RaceTrackData& data);
private:
    static QString serializeToYaml(const RaceTrackData& data);
    static bool deserializeFromYaml(const QString& yaml, RaceTrackData& data);
};

#endif // TALLER_TP_TRACK_SERIALIZER_H