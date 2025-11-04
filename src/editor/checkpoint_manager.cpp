#include "checkpoint_manager.h"

#include <QBrush>
#include <QFont>
#include <QGraphicsEllipseItem>
#include <QPen>

CheckpointManager::CheckpointManager(QGraphicsScene* scene)
    : scene(scene) {}

CheckpointManager::~CheckpointManager() {
    clear();
}

void CheckpointManager::addCheckpoint(const QPointF& pos) {
    Checkpoint cp;
    cp.position = pos;
    cp.order = static_cast<int>(checkpoints.size() + 1);

    cp.circle = scene->addEllipse(
        pos.x() - CHECKPOINT_RADIUS, 
        pos.y() - CHECKPOINT_RADIUS,
        CHECKPOINT_RADIUS * 2, 
        CHECKPOINT_RADIUS * 2,
        QPen(Qt::black, 2),
        QBrush(Qt::red)
    );
    cp.circle->setZValue(10);

    cp.label = scene->addText(QString::number(cp.order));
    cp.label->setDefaultTextColor(Qt::white);
    
    QFont font = cp.label->font();
    font.setBold(true);
    font.setPointSize(12);
    cp.label->setFont(font);
    cp.label->setZValue(11);

    QRectF textRect = cp.label->boundingRect();
    cp.label->setPos(
        pos.x() - textRect.width() / 2,
        pos.y() - textRect.height() / 2
    );
    checkpoints.push_back(cp);
}

void CheckpointManager::removeLastCheckpoint() {
    if (checkpoints.empty()) {
        return;
    }

    const auto& cp = checkpoints.back();
    scene->removeItem(cp.circle);
    scene->removeItem(cp.label);
    delete cp.circle;
    delete cp.label;
    checkpoints.pop_back();
}

void CheckpointManager::clear() {
    for (const auto& cp : checkpoints) {
        scene->removeItem(cp.circle);
        scene->removeItem(cp.label);
        delete cp.circle;
        delete cp.label;
    }
    checkpoints.clear();
}