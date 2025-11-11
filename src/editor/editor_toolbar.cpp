#include "editor_toolbar.h"
#include <QLabel>

EditorToolbar::EditorToolbar(QWidget* parent)
    : QToolBar("Herramientas", parent) {
    setMovable(false);

    setStyleSheet(
        "QToolBar {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 rgba(26, 26, 46, 230), stop:1 rgba(22, 33, 62, 230));"
        "   border-bottom: 3px solid #00d4ff;"
        "   spacing: 8px;"
        "   padding: 8px;"
        "}"
        "QLabel {"
        "   color: #00d4ff;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   background: transparent;"
        "   padding: 0px 10px;"
        "}"
        "QPushButton {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 rgba(40, 40, 60, 200), stop:1 rgba(30, 30, 46, 200));"
        "   color: #ffffff;"
        "   border: 2px solid #00d4ff;"
        "   border-radius: 8px;"
        "   padding: 8px 16px;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "   min-width: 100px;"
        "}"
        "QPushButton:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 rgba(50, 50, 80, 220), stop:1 rgba(40, 40, 60, 220));"
        "   border: 2px solid #00f5ff;"
        "   color: #00f5ff;"
        "}"
        "QPushButton:pressed {"
        "   background: rgba(20, 20, 36, 230);"
        "   border: 2px solid #0099cc;"
        "}"
        "QPushButton:checked {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #00d4ff, stop:1 #0099cc);"
        "   color: #000000;"
        "   border: 2px solid #00f5ff;"
        "}"
        "QToolBar::separator {"
        "   background: #00d4ff;"
        "   width: 2px;"
        "   margin: 5px 10px;"
        "}"
    );

    setupButtons();
}

void EditorToolbar::setupButtons() {
    QLabel *toolsLabel = new QLabel(" 🛠️ HERRAMIENTAS ");
    addWidget(toolsLabel);

    btnCheckpoint = new QPushButton("📍 Checkpoint");
    btnCheckpoint->setCheckable(true);
    btnCheckpoint->setToolTip("Coloca puntos de control en el mapa");
    addWidget(btnCheckpoint);
    connect(btnCheckpoint, &QPushButton::clicked, this, &EditorToolbar::checkpointModeRequested);

    addSeparator();

    btnUndo = new QPushButton("↶ Deshacer");
    btnUndo->setToolTip("Elimina el último checkpoint");
    addWidget(btnUndo);
    connect(btnUndo, &QPushButton::clicked, this, &EditorToolbar::undoRequested);

    btnClear = new QPushButton("🗑️ Limpiar");
    btnClear->setToolTip("Elimina todos los checkpoints y hints");
    addWidget(btnClear);
    connect(btnClear, &QPushButton::clicked, this, &EditorToolbar::clearRequested);

    addSeparator();

    btnSave = new QPushButton("💾 Guardar");
    btnSave->setToolTip("Guarda el recorrido");
    addWidget(btnSave);
    connect(btnSave, &QPushButton::clicked, this, &EditorToolbar::saveRequested);

    btnLoad = new QPushButton("📂 Cargar");
    btnLoad->setToolTip("Carga un recorrido existente");
    addWidget(btnLoad);
    connect(btnLoad, &QPushButton::clicked, this, &EditorToolbar::loadRequested);

    addSeparator();

    QLabel *zoomLabel = new QLabel(" 🔍 ZOOM ");
    addWidget(zoomLabel);

    btnZoomIn = new QPushButton("＋");
    btnZoomIn->setToolTip("Aumentar zoom");
    btnZoomIn->setStyleSheet(btnZoomIn->styleSheet() + "QPushButton { min-width: 40px; font-size: 18px; }");
    addWidget(btnZoomIn);
    connect(btnZoomIn, &QPushButton::clicked, this, &EditorToolbar::zoomInRequested);

    btnZoomOut = new QPushButton("－");
    btnZoomOut->setToolTip("Reducir zoom");
    btnZoomOut->setStyleSheet(btnZoomOut->styleSheet() + "QPushButton { min-width: 40px; font-size: 18px; }");
    addWidget(btnZoomOut);
    connect(btnZoomOut, &QPushButton::clicked, this, &EditorToolbar::zoomOutRequested);

    btnResetZoom = new QPushButton("⟲ Reset");
    btnResetZoom->setToolTip("Resetear zoom");
    addWidget(btnResetZoom);
    connect(btnResetZoom, &QPushButton::clicked, this, &EditorToolbar::resetZoomRequested);
}

void EditorToolbar::setCheckpointMode(bool active) {
    btnCheckpoint->setChecked(active);
}