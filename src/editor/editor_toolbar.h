#ifndef TALLER_TP_EDITOR_TOOLBAR_H
#define TALLER_TP_EDITOR_TOOLBAR_H

#include <QToolBar>
#include <QPushButton>

class EditorToolbar : public QToolBar {
    Q_OBJECT

public:
    explicit EditorToolbar(QWidget* parent = nullptr);

    void setCheckpointMode(bool active);
    void setHintMode(bool active);

    signals:
        void checkpointModeRequested();
        void hintModeRequested();
        void undoRequested();
        void clearRequested();
        void saveRequested();
        void loadRequested();
        void zoomInRequested();
        void zoomOutRequested();
        void resetZoomRequested();

private:
    QPushButton* btnCheckpoint;
    QPushButton* btnHint;
    QPushButton* btnUndo;
    QPushButton* btnClear;
    QPushButton* btnSave;
    QPushButton* btnLoad;
    QPushButton* btnZoomIn;
    QPushButton* btnZoomOut;
    QPushButton* btnResetZoom;

    void setupButtons();
};

#endif // TALLER_TP_EDITOR_TOOLBAR_H