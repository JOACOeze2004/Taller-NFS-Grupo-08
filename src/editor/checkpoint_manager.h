#ifndef TALLER_TP_CHECKPOINT_MANAGER_H
#define TALLER_TP_CHECKPOINT_MANAGER_H

#include <QGraphicsScene>
#include <QPointF>
#include <QPixmap>
#include <vector>

class CheckpointManager {
public:
    enum class CheckpointType {
        START,
        INTERMEDIATE,
        FINISH
    };

    struct Checkpoint {
        QGraphicsEllipseItem* circle;
        QGraphicsTextItem* label;
        QGraphicsPixmapItem* icon;
        QPointF position;
        int order;
        CheckpointType type;
    };

    explicit CheckpointManager(QGraphicsScene* scene);
    ~CheckpointManager();

    void addCheckpoint(const QPointF& pos);
    void removeCheckpoint(QGraphicsEllipseItem* circle);
    void moveCheckpoint(QGraphicsEllipseItem* circle, const QPointF& newPos);
    void clear();

    QGraphicsEllipseItem* getCheckpointAt(const QPointF& scenePos);

    const std::vector<Checkpoint>& getCheckpoints() const { return checkpoints; }
    size_t count() const { return checkpoints.size(); }
    bool hasMinimumCheckpoints() const { return checkpoints.size() >= 2; }

private:
    QGraphicsScene* scene;
    std::vector<Checkpoint> checkpoints;

    static constexpr qreal CHECKPOINT_RADIUS = 20.0;

    void updateCheckpointTypes();
    void updateCheckpointAppearance(Checkpoint& cp);
    void updateCheckpointLabel(Checkpoint& cp);
    QColor getCheckpointColor(CheckpointType type) const;
};

#endif // TALLER_TP_CHECKPOINT_MANAGER_H