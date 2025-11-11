#ifndef TALLER_TP_COLLISION_VALIDATOR_H
#define TALLER_TP_COLLISION_VALIDATOR_H

#include <QString>
#include <QPointF>
#include <QRectF>
#include <vector>

class CollisionValidator {
public:
    struct CollisionRect {
        QRectF rect;
        QString type;
    };

    explicit CollisionValidator();
    bool loadCollisionData(const QString& cityId);
    bool isPointValid(const QPointF& point) const;
    struct ValidationResult {
        bool valid;
        QString errorMessage;
        QPointF invalidPoint;
    };
    
    ValidationResult validateCheckpoints(const std::vector<QPointF>& checkpoints) const;
    ValidationResult validateHints(const std::vector<QPointF>& hints) const;
    void clear();

private:
    std::vector<CollisionRect> collisionRects;
    QString currentCity;
    bool parseCollisionFile(const QString& filePath);
    bool isPointInRect(const QPointF& point, const QRectF& rect) const;
};

#endif // TALLER_TP_COLLISION_VALIDATOR_H