#include "hint_manager.h"
#include <QPen>
#include <QBrush>

HintManager::HintManager(QGraphicsScene* scene)
    : scene(scene) {}

HintManager::~HintManager() {
    clear();
}

QPolygonF HintManager::createArrowShape() {
    QPolygonF arrow;
    arrow << QPointF(0, -30)
          << QPointF(-15, 0)
          << QPointF(-8, 0)
          << QPointF(-8, 20)
          << QPointF(8, 20)
          << QPointF(8, 0)
          << QPointF(15, 0);
    return arrow;
}

void HintManager::addHint(const QPointF& pos, qreal rotation) {
    Hint hint;
    hint.position = pos;
    hint.rotation = rotation;

    const QPolygonF arrowShape = createArrowShape();
    hint.arrow = scene->addPolygon(
        arrowShape,
        QPen(Qt::black, 2),
        QBrush(Qt::green)
    );

    hint.arrow->setPos(pos);
    hint.arrow->setRotation(rotation);
    hint.arrow->setOpacity(0.7);
    hint.arrow->setZValue(5);

    hints.push_back(hint);
}

QGraphicsPolygonItem* HintManager::createTempArrow(const QPointF& pos) const {
    const QPolygonF arrowShape = createArrowShape();
    QGraphicsPolygonItem* tempArrow = scene->addPolygon(
        arrowShape,
        QPen(Qt::black, 2),
        QBrush(Qt::yellow)
    );

    tempArrow->setPos(pos);
    tempArrow->setOpacity(0.5);
    tempArrow->setZValue(15);

    return tempArrow;
}

void HintManager::removeLastHint() {
    if (hints.empty()) {
        return;
    }

    const auto& hint = hints.back();
    scene->removeItem(hint.arrow);
    delete hint.arrow;
    hints.pop_back();
}

void HintManager::clear() {
    for (const auto& hint : hints) {
        scene->removeItem(hint.arrow);
        delete hint.arrow;
    }
    hints.clear();
}