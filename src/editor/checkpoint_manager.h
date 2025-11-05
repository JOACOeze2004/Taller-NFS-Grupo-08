#ifndef TALLER_TP_CHECKPOINT_MANAGER_H
#define TALLER_TP_CHECKPOINT_MANAGER_H

#include <QGraphicsScene>
#include <QPointF>
#include <vector>

class CheckpointManager {
public:
    struct Checkpoint {
        QGraphicsEllipseItem* circle;
        QGraphicsTextItem* label;
        QPointF position;
        int order;
    };

    explicit CheckpointManager(QGraphicsScene* scene);
    ~CheckpointManager();

    void addCheckpoint(const QPointF& pos);
    void removeLastCheckpoint();
    void clear();
    const std::vector<Checkpoint>& getCheckpoints() const { return checkpoints; }
    size_t count() const { return checkpoints.size(); }

private:
    QGraphicsScene* scene;
    std::vector<Checkpoint> checkpoints;

    static constexpr qreal CHECKPOINT_RADIUS = 20.0;
};

#endif // TALLER_TP_CHECKPOINT_MANAGER_H