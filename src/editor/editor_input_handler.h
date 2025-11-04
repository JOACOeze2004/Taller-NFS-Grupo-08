#ifndef TALLER_TP_EDITOR_INPUT_HANDLER_H
#define TALLER_TP_EDITOR_INPUT_HANDLER_H

#include <QObject>
#include <QPointF>
#include <QMouseEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPolygonItem>

#include "checkpoint_manager.h"
#include "hint_manager.h"

class EditorInputHandler : public QObject {
    Q_OBJECT

public:
    enum class EditMode {
        NONE,
        CHECKPOINT,
        HINT
    };

    explicit EditorInputHandler(QGraphicsView* view, QGraphicsScene* scene, CheckpointManager* checkpointMgr, HintManager* hintMgr, QObject* parent = nullptr);

    void setMode(EditMode mode);
    EditMode getMode() const { return currentMode; }

    bool handleMousePress(QMouseEvent* event);
    bool handleMouseMove(QMouseEvent* event);
    bool handleMouseRelease(QMouseEvent* event);

    signals:
        void checkpointAdded(const QPointF& pos);
    void hintAdded(const QPointF& pos, qreal rotation);
    void modeCancelled();
    void hintDragStarted();
    void hintDragUpdated(qreal angle);

private:
    QGraphicsView* view;
    QGraphicsScene* scene;
    CheckpointManager* checkpointManager;
    HintManager* hintManager;

    EditMode currentMode;
    bool isDraggingHint;
    QPointF hintStartPos;
    QGraphicsPolygonItem* tempArrow;

    void handleCheckpointClick(const QPointF& scenePos);
    void handleHintStart(const QPointF& scenePos);
    void handleHintDrag(const QPointF& scenePos);
    void handleHintEnd(const QPointF& scenePos);
    void cancelHintDrag();
};

#endif // TALLER_TP_EDITOR_INPUT_HANDLER_H