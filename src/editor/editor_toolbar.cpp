#include "editor_toolbar.h"
#include <QLabel>

EditorToolbar::EditorToolbar(QWidget* parent)
    : QToolBar("Herramientas", parent) {
    setMovable(false);
    setupButtons();
}

void EditorToolbar::setupButtons() {
    addWidget(new QLabel(" Herramientas: "));

    btnCheckpoint = new QPushButton("Checkpoint");
    btnCheckpoint->setCheckable(true);
    btnCheckpoint->setToolTip("Coloca puntos de control en el mapa");
    addWidget(btnCheckpoint);
    connect(btnCheckpoint, &QPushButton::clicked, this, &EditorToolbar::checkpointModeRequested);

    btnHint = new QPushButton("Hint");
    btnHint->setCheckable(true);
    btnHint->setToolTip("Coloca flechas direccionales");
    addWidget(btnHint);
    connect(btnHint, &QPushButton::clicked, this, &EditorToolbar::hintModeRequested);

    addSeparator();

    btnUndo = new QPushButton("Deshacer");
    btnUndo->setToolTip("Elimina el último elemento");
    addWidget(btnUndo);
    connect(btnUndo, &QPushButton::clicked, this, &EditorToolbar::undoRequested);

    btnClear = new QPushButton("Limpiar");
    btnClear->setToolTip("Elimina todos los checkpoints y hints");
    addWidget(btnClear);
    connect(btnClear, &QPushButton::clicked, this, &EditorToolbar::clearRequested);

    addSeparator();

    btnSave = new QPushButton("Guardar");
    btnSave->setToolTip("Guarda el recorrido ");
    addWidget(btnSave);
    connect(btnSave, &QPushButton::clicked, this, &EditorToolbar::saveRequested);

    btnLoad = new QPushButton("Cargar");
    btnLoad->setToolTip("Carga un recorrido existente ");
    addWidget(btnLoad);
    connect(btnLoad, &QPushButton::clicked, this, &EditorToolbar::loadRequested);

    addSeparator();
    addWidget(new QLabel(" Zoom: "));

    btnZoomIn = new QPushButton("Zoom +");
    btnZoomIn->setToolTip("Aumentar zoom");
    addWidget(btnZoomIn);
    connect(btnZoomIn, &QPushButton::clicked, this, &EditorToolbar::zoomInRequested);

    btnZoomOut = new QPushButton("Zoom -");
    btnZoomOut->setToolTip("Reducir zoom");
    addWidget(btnZoomOut);
    connect(btnZoomOut, &QPushButton::clicked, this, &EditorToolbar::zoomOutRequested);

    btnResetZoom = new QPushButton("Reset Zoom");
    btnResetZoom->setToolTip("Resetear zoom");
    addWidget(btnResetZoom);
    connect(btnResetZoom, &QPushButton::clicked, this, &EditorToolbar::resetZoomRequested);
}

void EditorToolbar::setCheckpointMode(bool active) {
    btnCheckpoint->setChecked(active);
    btnHint->setChecked(false);
}

void EditorToolbar::setHintMode(bool active) {
    btnHint->setChecked(active);
    btnCheckpoint->setChecked(false);
}