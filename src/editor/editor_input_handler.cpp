#include "editor_input_handler.h"
#include <QMessageBox>
#include <cmath>

EditorInputHandler::EditorInputHandler(QGraphicsView* view, QGraphicsScene* scene,
                                       CheckpointManager* checkpointMgr, HintManager* hintMgr,
                                       QObject* parent)
    : QObject(parent), view(view), scene(scene), checkpointManager(checkpointMgr),
      hintManager(hintMgr), currentMode(EditMode::NONE),
      isDraggingCheckpoint(false), draggedCheckpoint(nullptr) {}

void EditorInputHandler::setMode(EditMode mode) {
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
        if (currentMode == EditMode::NONE) {
            if (QGraphicsEllipseItem* checkpoint = checkpointManager->getCheckpointAt(scenePos)) {
                handleCheckpointDragStart(checkpoint, scenePos);
                return true;
            }
            return false;
        }

        switch (currentMode) {
            case EditMode::CHECKPOINT:
                handleCheckpointClick(scenePos);
                return true;

            case EditMode::NONE:
                return false;
            default:;
        }
    }
    else if (event->button() == Qt::RightButton) {
        if (QGraphicsEllipseItem* checkpoint = checkpointManager->getCheckpointAt(scenePos)) {
            showCheckpointContextMenu(checkpoint, event->globalPos());
            return true;
        }

        setMode(EditMode::NONE);
        emit modeCancelled();
        return true;
    }
    return false;
}

bool EditorInputHandler::handleMouseMove(QMouseEvent* event) {
    QPointF scenePos = view->mapToScene(event->pos());

    if (isDraggingCheckpoint && draggedCheckpoint) {
        handleCheckpointDrag(scenePos);
        return true;
    }

    return false;
}

bool EditorInputHandler::handleMouseRelease(QMouseEvent* event) {
    QPointF scenePos = view->mapToScene(event->pos());

    if (event->button() == Qt::LeftButton) {
        if (isDraggingCheckpoint) {
            handleCheckpointDragEnd(scenePos);
            return true;
        }
    }

    return false;
}

bool EditorInputHandler::handleContextMenu(QContextMenuEvent* event) {
    QPointF scenePos = view->mapToScene(event->pos());

    if (QGraphicsEllipseItem* checkpoint = checkpointManager->getCheckpointAt(scenePos)) {
        showCheckpointContextMenu(checkpoint, event->globalPos());
        return true;
    }

    return false;
}

void EditorInputHandler::handleCheckpointClick(const QPointF& scenePos) {
    checkpointManager->addCheckpoint(scenePos);
    emit checkpointAdded(scenePos);
}

void EditorInputHandler::handleCheckpointDragStart(QGraphicsEllipseItem* checkpoint, const QPointF& scenePos) {
    isDraggingCheckpoint = true;
    draggedCheckpoint = checkpoint;
    dragStartPos = scenePos;
    view->setDragMode(QGraphicsView::NoDrag);
}

void EditorInputHandler::handleCheckpointDrag(const QPointF& scenePos) {
    if (draggedCheckpoint) {
        checkpointManager->moveCheckpoint(draggedCheckpoint, scenePos);
    }
}

void EditorInputHandler::handleCheckpointDragEnd(const QPointF& scenePos) {
    if (draggedCheckpoint) {
        checkpointManager->moveCheckpoint(draggedCheckpoint, scenePos);
        emit checkpointMoved(scenePos);
    }

    isDraggingCheckpoint = false;
    draggedCheckpoint = nullptr;

    if (currentMode == EditMode::NONE) {
        view->setDragMode(QGraphicsView::ScrollHandDrag);
    }
}

void EditorInputHandler::showCheckpointContextMenu(QGraphicsEllipseItem* checkpoint, const QPoint& screenPos) {
    QMenu menu;
    QAction* deleteAction = menu.addAction("Eliminar Checkpoint");

    QAction* selected = menu.exec(screenPos);
    if (selected == deleteAction) {
        checkpointManager->removeCheckpoint(checkpoint);
        emit checkpointRemoved();
    }
}