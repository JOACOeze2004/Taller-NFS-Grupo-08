#include "editor_input_handler.h"
#include <cmath>

EditorInputHandler::EditorInputHandler(QGraphicsView* view, QGraphicsScene* scene, CheckpointManager* checkpointMgr, HintManager* hintMgr, QObject* parent)
    : QObject(parent), view(view), scene(scene), checkpointManager(checkpointMgr),hintManager(hintMgr), currentMode(EditMode::NONE), isDraggingHint(false), tempArrow(nullptr) {}

void EditorInputHandler::setMode(EditMode mode) {
    if (isDraggingHint) {
        cancelHintDrag();
    }
    currentMode = mode;

    if (mode == EditMode::NONE) {
        view->setDragMode(QGraphicsView::ScrollHandDrag);
    } else {
        view->setDragMode(QGraphicsView::NoDrag);
    }
}

bool EditorInputHandler::handleMousePress(QMouseEvent* event) {
    QPointF scenePos = view->mapToScene(event->pos());
    if (event->button() == Qt::LeftButton) {
        switch (currentMode) {
            case EditMode::CHECKPOINT:
                handleCheckpointClick(scenePos);
                return true;

            case EditMode::HINT:
                handleHintStart(scenePos);
                return true;

            case EditMode::NONE:
                return false;
        }
    }
    else if (event->button() == Qt::RightButton) {
        if (isDraggingHint) {
            cancelHintDrag();
        }
        setMode(EditMode::NONE);
        emit modeCancelled();
        return true;
    }
    return false;
}

bool EditorInputHandler::handleMouseMove(QMouseEvent* event) {
    if (isDraggingHint && tempArrow) {
        QPointF scenePos = view->mapToScene(event->pos());
        handleHintDrag(scenePos);
        return true;
    }
    return false;
}

bool EditorInputHandler::handleMouseRelease(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && isDraggingHint) {
        QPointF scenePos = view->mapToScene(event->pos());
        handleHintEnd(scenePos);
        return true;
    }
    return false;
}

void EditorInputHandler::handleCheckpointClick(const QPointF& scenePos) {
    checkpointManager->addCheckpoint(scenePos);
    emit checkpointAdded(scenePos);
}

void EditorInputHandler::handleHintStart(const QPointF& scenePos) {
    isDraggingHint = true;
    hintStartPos = scenePos;
    tempArrow = hintManager->createTempArrow(scenePos);
    emit hintDragStarted();
}

void EditorInputHandler::handleHintDrag(const QPointF& scenePos) {
    qreal dx = scenePos.x() - hintStartPos.x();
    qreal dy = scenePos.y() - hintStartPos.y();
    qreal angle = std::atan2(dy, dx) * 180.0 / M_PI + 90;

    tempArrow->setRotation(angle);
    emit hintDragUpdated(angle);
}

void EditorInputHandler::handleHintEnd(const QPointF& scenePos) {
    qreal dx = scenePos.x() - hintStartPos.x();
    qreal dy = scenePos.y() - hintStartPos.y();
    qreal angle = std::atan2(dy, dx) * 180.0 / M_PI + 90;

    scene->removeItem(tempArrow);
    delete tempArrow;
    tempArrow = nullptr;

    hintManager->addHint(hintStartPos, angle);
    emit hintAdded(hintStartPos, angle);

    isDraggingHint = false;
}

void EditorInputHandler::cancelHintDrag() {
    if (tempArrow) {
        scene->removeItem(tempArrow);
        delete tempArrow;
        tempArrow = nullptr;
    }
    isDraggingHint = false;
}