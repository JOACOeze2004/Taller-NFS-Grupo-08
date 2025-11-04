#ifndef TALLER_TP_RACE_TRACK_DATA_H
#define TALLER_TP_RACE_TRACK_DATA_H

#include <QString>
#include <QPointF>
#include <vector>

struct CheckpointData {
    QPointF position;
    int order;
};

struct HintData {
    QPointF position;
    qreal rotation;
};

struct RaceTrackData {
    QString cityId;
    std::vector<CheckpointData> checkpoints;
    std::vector<HintData> hints;
    QString trackName;
    QString author;
    QString createdDate;
};

#endif // TALLER_TP_RACE_TRACK_DATA_H