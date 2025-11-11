#include "hint_generator.h"
#include <cmath>

void HintGenerator::generateHintsForTrack(
    const std::vector<CheckpointManager::Checkpoint>& checkpoints,
    HintManager* hintManager
) {
    hintManager->clear();
    
    if (checkpoints.size() < 2) {
        return;
    }

    for (size_t i = 0; i < checkpoints.size() - 1; ++i) {
        const QPointF& start = checkpoints[i].position;
        const QPointF& end = checkpoints[i + 1].position;
        
        QPointF midPoint = (start + end) / 2.0;
        qreal angle = calculateAngle(start, end);
        hintManager->addHint(midPoint, angle);
    }
}

qreal HintGenerator::calculateAngle(const QPointF& from, const QPointF& to) {
    qreal dx = to.x() - from.x();
    qreal dy = to.y() - from.y();
    return std::atan2(dy, dx) * 180.0 / M_PI + 90.0;
}

std::vector<QPointF> HintGenerator::interpolatePoints(
    const QPointF& start, 
    const QPointF& end, 
    int numPoints
) {
    std::vector<QPointF> points;
    
    for (int i = 1; i <= numPoints; ++i) {
        qreal t = static_cast<qreal>(i) / (numPoints + 1);
        QPointF point = start + (end - start) * t;
        points.push_back(point);
    }
    
    return points;
}

qreal HintGenerator::calculateCurveAngle(
    const QPointF& prev, 
    const QPointF& current, 
    const QPointF& next
) {

    qreal angle1 = calculateAngle(prev, current);
    qreal angle2 = calculateAngle(current, next);

    qreal diff = angle2 - angle1;
    if (diff > 180) diff -= 360;
    if (diff < -180) diff += 360;
    
    return angle1 + diff * 0.5;
}