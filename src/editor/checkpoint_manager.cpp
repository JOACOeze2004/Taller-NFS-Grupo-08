#include "checkpoint_manager.h"
#include <QBrush>
#include <QFont>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <algorithm>

CheckpointManager::CheckpointManager(QGraphicsScene* scene)
    : scene(scene) {}

CheckpointManager::~CheckpointManager() {
    clear();
}

void CheckpointManager::addCheckpoint(const QPointF& pos) {
    Checkpoint cp;
    cp.position = pos;
    cp.order = static_cast<int>(checkpoints.size() + 1);
    cp.type = CheckpointType::INTERMEDIATE;
    cp.icon = nullptr;

    cp.circle = scene->addEllipse(
        pos.x() - CHECKPOINT_RADIUS,
        pos.y() - CHECKPOINT_RADIUS,
        CHECKPOINT_RADIUS * 2,
        CHECKPOINT_RADIUS * 2,
        QPen(Qt::black, 2),
        QBrush(getCheckpointColor(cp.type))
    );
    cp.circle->setZValue(10);
    cp.circle->setFlag(QGraphicsItem::ItemIsSelectable, true);
    cp.circle->setFlag(QGraphicsItem::ItemIsMovable, false);

    cp.label = scene->addText(QString::number(cp.order));
    cp.label->setDefaultTextColor(Qt::white);

    QFont font = cp.label->font();
    font.setBold(true);
    font.setPointSize(12);
    cp.label->setFont(font);
    cp.label->setZValue(11);

    updateCheckpointLabel(cp);
    checkpoints.push_back(cp);

    updateCheckpointTypes();
}

void CheckpointManager::removeCheckpoint(QGraphicsEllipseItem* circle) {
    auto it = std::find_if(checkpoints.begin(), checkpoints.end(),
        [circle](const Checkpoint& cp) { return cp.circle == circle; });

    if (it != checkpoints.end()) {
        scene->removeItem(it->circle);
        scene->removeItem(it->label);
        if (it->icon) {
            scene->removeItem(it->icon);
            delete it->icon;
        }
        delete it->circle;
        delete it->label;
        checkpoints.erase(it);

        for (size_t i = 0; i < checkpoints.size(); ++i) {
            checkpoints[i].order = static_cast<int>(i + 1);
            checkpoints[i].label->setPlainText(QString::number(checkpoints[i].order));
            updateCheckpointLabel(checkpoints[i]);
        }

        updateCheckpointTypes();
    }
}

void CheckpointManager::moveCheckpoint(QGraphicsEllipseItem* circle, const QPointF& newPos) {

    auto it = std::find_if(checkpoints.begin(), checkpoints.end(),
        [circle](const Checkpoint& cp) { return cp.circle == circle; });

    if (it != checkpoints.end()) {
        it->position = newPos;
        it->circle->setPos(0, 0);
        it->circle->setRect(
            newPos.x() - CHECKPOINT_RADIUS,
            newPos.y() - CHECKPOINT_RADIUS,
            CHECKPOINT_RADIUS * 2,
            CHECKPOINT_RADIUS * 2
        );
        updateCheckpointLabel(*it);

        if (it->icon) {
            it->icon->setPos(newPos.x() - 10, newPos.y() - 10);
        }
    }
}

QGraphicsEllipseItem* CheckpointManager::getCheckpointAt(const QPointF& scenePos) {
    for (const auto& cp : checkpoints) {
        QPointF center = cp.position;
        qreal distance = QLineF(center, scenePos).length();
        if (distance <= CHECKPOINT_RADIUS) {
            return cp.circle;
        }
    }
    return nullptr;
}

void CheckpointManager::clear() {
    for (const auto& cp : checkpoints) {
        scene->removeItem(cp.circle);
        scene->removeItem(cp.label);
        if (cp.icon) {
            scene->removeItem(cp.icon);
            delete cp.icon;
        }
        delete cp.circle;
        delete cp.label;
    }
    checkpoints.clear();
}

void CheckpointManager::updateCheckpointTypes() {
    if (checkpoints.empty()) {
        return;
    }

    checkpoints[0].type = CheckpointType::START;

    if (checkpoints.size() > 1) {
        checkpoints.back().type = CheckpointType::FINISH;
    }

    for (size_t i = 1; i < checkpoints.size() - 1; ++i) {
        checkpoints[i].type = CheckpointType::INTERMEDIATE;
    }

    for (auto& cp : checkpoints) {
        updateCheckpointAppearance(cp);
    }
}

void CheckpointManager::updateCheckpointAppearance(Checkpoint& cp) {
    QColor color = getCheckpointColor(cp.type);
    cp.circle->setBrush(QBrush(color));

    QString labelText;
    if (cp.type == CheckpointType::START) {
        labelText = "S";
        cp.label->setDefaultTextColor(Qt::white);
    } else if (cp.type == CheckpointType::FINISH) {
        labelText = "F";
        cp.label->setDefaultTextColor(Qt::white);
    } else {
        labelText = QString::number(cp.order - 1);
        cp.label->setDefaultTextColor(Qt::white);
    }

    cp.label->setPlainText(labelText);
    updateCheckpointLabel(cp);
}

void CheckpointManager::updateCheckpointLabel(Checkpoint& cp) {
    QRectF textRect = cp.label->boundingRect();
    cp.label->setPos(
        cp.position.x() - textRect.width() / 2,
        cp.position.y() - textRect.height() / 2
    );
}

QColor CheckpointManager::getCheckpointColor(CheckpointType type) const {
    switch (type) {
        case CheckpointType::START:
            return Qt::green;
        case CheckpointType::FINISH:
            return Qt::red;
        case CheckpointType::INTERMEDIATE:
            return Qt::blue;
    }
    return Qt::gray;
}