#ifndef TALLER_TP_HINT_GENERATOR_H
#define TALLER_TP_HINT_GENERATOR_H

#include <QPointF>
#include <vector>
#include "checkpoint_manager.h"
#include "hint_manager.h"

class HintGenerator {
public:
    static void generateHintsForTrack(
        const std::vector<CheckpointManager::Checkpoint>& checkpoints,
        HintManager* hintManager
    );

private:
    static qreal calculateAngle(const QPointF& from, const QPointF& to);
    static std::vector<QPointF> interpolatePoints(const QPointF& start, const QPointF& end, int numPoints);
    static qreal calculateCurveAngle(const QPointF& prev, const QPointF& current, const QPointF& next);
};

#endif // TALLER_TP_HINT_GENERATOR_H