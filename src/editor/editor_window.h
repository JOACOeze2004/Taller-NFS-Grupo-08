#ifndef TALLER_TP_EDITOR_WINDOW_H
#define TALLER_TP_EDITOR_WINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>

#include "map_renderer.h"
#include "checkpoint_manager.h"
#include "hint_manager.h"
#include "editor_toolbar.h"
#include "editor_input_handler.h"
#include "race_track_data.h"
#include "collision_validator.h"

class EditorWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit EditorWindow(const QString& cityName, QWidget* parent = nullptr);
    ~EditorWindow() override = default;

private slots:
    void onCheckpointModeRequested();
    void onUndoRequested();
    void onClearRequested();
    void onSaveRequested();
    void onLoadRequested();

    void onCheckpointAdded(const QPointF& pos);
    void onCheckpointRemoved();
    void onCheckpointMoved(const QPointF& pos);
    void onModeCancelled();

    void onGenerateHints();

    void onZoomIn();
    void onZoomOut();
    void onResetZoom();

private:
    QGraphicsScene* scene;
    QGraphicsView* view;
    QString currentCity;

    MapRenderer* mapRenderer;
    CheckpointManager* checkpointManager;
    HintManager* hintManager;
    EditorToolbar* toolbar;
    EditorInputHandler* inputHandler;
    CollisionValidator* collisionValidator;

    void setupScene();
    void setupView();
    void setupToolbar();
    void setupConnections();
    void drawGrid();

    QString getCityDisplayName() const;
    void updateStatusMessage(const QString& message);

    bool validateTrack() const;
    RaceTrackData collectTrackData() const;
    void restoreTrackData(const RaceTrackData& data);
    bool confirmDiscard();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // TALLER_TP_EDITOR_WINDOW_H