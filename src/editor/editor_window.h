#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>

#include "map_renderer.h"
#include "checkpoint_manager.h"
#include "hint_manager.h"
#include "editor_toolbar.h"
#include "editor_input_handler.h"
#include "race_track_data.h"

class EditorWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit EditorWindow(QString  cityName, QWidget* parent = nullptr);
    ~EditorWindow() override = default;

private slots:
    void onCheckpointModeRequested() const;
    void onHintModeRequested() const;
    void onUndoRequested() const;
    void onClearRequested();
    void onSaveRequested();
    void onLoadRequested();

    void onCheckpointAdded(const QPointF& pos) const;
    void onHintAdded(const QPointF& pos, qreal rotation) const;
    void onModeCancelled() const;
    void onHintDragUpdated(qreal angle) const;

    void onZoomIn() const;
    void onZoomOut() const;
    void onResetZoom() const;

private:
    QGraphicsScene* scene;
    QGraphicsView* view;
    QString currentCity;

    MapRenderer* mapRenderer;
    CheckpointManager* checkpointManager;
    HintManager* hintManager;
    EditorToolbar* toolbar;
    EditorInputHandler* inputHandler;

    void setupScene();
    void setupView();
    void setupToolbar();
    void setupConnections();
    void drawGrid() const;

    QString getCityDisplayName() const;
    void updateStatusMessage(const QString& message) const;
    RaceTrackData collectTrackData() const;
    void restoreTrackData(const RaceTrackData& data) const;
    bool confirmDiscard();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};

#endif // EDITOR_WINDOW_H