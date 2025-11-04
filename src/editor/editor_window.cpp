#include "editor_window.h"
#include "track_file_manager.h"

#include <QVBoxLayout>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QStatusBar>
#include <utility>

EditorWindow::EditorWindow(QString  cityName, QWidget* parent)
    : QMainWindow(parent), scene(nullptr), view(nullptr), currentCity(std::move(cityName)), mapRenderer(nullptr), checkpointManager(nullptr), hintManager(nullptr), toolbar(nullptr), inputHandler(nullptr)
{
    setWindowTitle(QString("Need for Speed - Editor: %1").arg(getCityDisplayName()));
    resize(1200, 800);

    setupScene();
    setupView();
    setupToolbar();
    setupConnections();

    updateStatusMessage("Selecciona una herramienta para comenzar");
}

void EditorWindow::setupScene() {
    scene = new QGraphicsScene(this);

    mapRenderer = new MapRenderer(scene);
    checkpointManager = new CheckpointManager(scene);
    hintManager = new HintManager(scene);

    if (!mapRenderer->loadCityMap(currentCity)) {
        QMessageBox::critical(this, "Error Crítico", QString("No se pudo cargar el mapa de %1.\n\n" "Verifica que existan las imágenes en:\n" "../assets/need-for-speed/cities/").arg(getCityDisplayName()));
        close();
        return;
    }
    drawGrid();
}

void EditorWindow::setupView() {
    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->viewport()->installEventFilter(this);
    setCentralWidget(view);
}

void EditorWindow::setupToolbar() {
    toolbar = new EditorToolbar(this);
    addToolBar(toolbar);
}

void EditorWindow::setupConnections() {
    inputHandler = new EditorInputHandler(view, scene, checkpointManager, hintManager, this);

    connect(toolbar, &EditorToolbar::checkpointModeRequested, this, &EditorWindow::onCheckpointModeRequested);
    connect(toolbar, &EditorToolbar::hintModeRequested, this, &EditorWindow::onHintModeRequested);
    connect(toolbar, &EditorToolbar::undoRequested, this, &EditorWindow::onUndoRequested);
    connect(toolbar, &EditorToolbar::clearRequested, this, &EditorWindow::onClearRequested);
    connect(toolbar, &EditorToolbar::saveRequested, this, &EditorWindow::onSaveRequested);
    connect(toolbar, &EditorToolbar::loadRequested, this, &EditorWindow::onLoadRequested);
    connect(toolbar, &EditorToolbar::zoomInRequested, this, &EditorWindow::onZoomIn);
    connect(toolbar, &EditorToolbar::zoomOutRequested, this, &EditorWindow::onZoomOut);
    connect(toolbar, &EditorToolbar::resetZoomRequested, this, &EditorWindow::onResetZoom);

    connect(inputHandler, &EditorInputHandler::checkpointAdded, this, &EditorWindow::onCheckpointAdded);
    connect(inputHandler, &EditorInputHandler::hintAdded, this, &EditorWindow::onHintAdded);
    connect(inputHandler, &EditorInputHandler::modeCancelled, this, &EditorWindow::onModeCancelled);
    connect(inputHandler, &EditorInputHandler::hintDragUpdated, this, &EditorWindow::onHintDragUpdated);
}

void EditorWindow::drawGrid() const {
    const QPen gridPen(QColor(200, 200, 200, 100), 1, Qt::DotLine);
    constexpr int gridSize = 50;
    const QRectF bounds = scene->sceneRect();

    for (int x = 0; x <= bounds.width(); x += gridSize) {
        QGraphicsLineItem* line = scene->addLine(x, 0, x, bounds.height(), gridPen);
        line->setZValue(-0.5);
    }

    for (int y = 0; y <= bounds.height(); y += gridSize) {
        QGraphicsLineItem* line = scene->addLine(0, y, bounds.width(), y, gridPen);
        line->setZValue(-0.5);
    }
}

QString EditorWindow::getCityDisplayName() const {
    if (currentCity == "liberty_city") return "Liberty City";
    if (currentCity == "san_andreas") return "San Andreas";
    if (currentCity == "vice_city") return "Vice City";
    return currentCity;
}

void EditorWindow::updateStatusMessage(const QString& message) const {
    statusBar()->showMessage(QString("%1 - %2").arg(getCityDisplayName(), message));
}

void EditorWindow::onCheckpointModeRequested() const {
    inputHandler->setMode(EditorInputHandler::EditMode::CHECKPOINT);
    toolbar->setCheckpointMode(true);
    updateStatusMessage("Modo Checkpoint - Click para colocar");
}

void EditorWindow::onHintModeRequested() const {
    inputHandler->setMode(EditorInputHandler::EditMode::HINT);
    toolbar->setHintMode(true);
    updateStatusMessage("Modo Hint - Click y arrastra para dirección");
}

void EditorWindow::onUndoRequested() const {

    if (const auto mode = inputHandler->getMode();
        mode == EditorInputHandler::EditMode::CHECKPOINT && checkpointManager->count() > 0) {
        checkpointManager->removeLastCheckpoint();
        updateStatusMessage(QString("Checkpoint eliminado (quedan %1)").arg(checkpointManager->count()));
    } else if (mode == EditorInputHandler::EditMode::HINT && hintManager->count() > 0) {
        hintManager->removeLastHint();
        updateStatusMessage(QString("Hint eliminado (quedan %1)").arg(hintManager->count()));
    } else if (hintManager->count() > 0) {
        hintManager->removeLastHint();
        updateStatusMessage(QString("Hint eliminado (quedan %1)").arg(hintManager->count()));
    } else if (checkpointManager->count() > 0) {
        checkpointManager->removeLastCheckpoint();
        updateStatusMessage(QString("Checkpoint eliminado (quedan %1)").arg(checkpointManager->count()));
    } else {
        updateStatusMessage("No hay nada que deshacer");
    }
}

void EditorWindow::onClearRequested() {
    if (checkpointManager->count() == 0 && hintManager->count() == 0) {
        updateStatusMessage("No hay nada que limpiar");
        return;
    }

    const auto reply = QMessageBox::question(this, "Limpiar Mapa",
        "¿Seguro que quieres eliminar todos los checkpoints y hints?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        checkpointManager->clear();
        hintManager->clear();
        inputHandler->setMode(EditorInputHandler::EditMode::NONE);
        toolbar->setCheckpointMode(false);
        toolbar->setHintMode(false);
        updateStatusMessage("Mapa limpiado");
    }
}

void EditorWindow::onSaveRequested() {
    if (RaceTrackData data = collectTrackData();
        TrackFileManager::saveTrackWithDialog(this, data, currentCity)) {
        updateStatusMessage(QString("Recorrido '%1' guardada").arg(data.trackName));
    }
}

void EditorWindow::onLoadRequested() {
    if (!confirmDiscard()) {
        return;
    }

    QString newCity = currentCity;

    if (RaceTrackData data; TrackFileManager::loadTrackWithDialog(this, data, newCity)) {
        if (newCity != currentCity) {
            currentCity = newCity;
            mapRenderer->loadCityMap(currentCity);
            setWindowTitle(QString("Need for Speed - Editor: %1").arg(getCityDisplayName()));
        }

        restoreTrackData(data);
        updateStatusMessage(QString("Recorrido '%1' cargada").arg(data.trackName));
    }
}

void EditorWindow::onCheckpointAdded(const QPointF& pos) const {
    updateStatusMessage(QString("Checkpoint %1 agregado en (%2, %3)")
        .arg(checkpointManager->count())
        .arg(static_cast<int>(pos.x()))
        .arg(static_cast<int>(pos.y())));
}

void EditorWindow::onHintAdded(const QPointF& pos, qreal rotation) const {
    updateStatusMessage(QString("Hint %1 agregado en (%2, %3) - %4°")
        .arg(hintManager->count())
        .arg(static_cast<int>(pos.x()))
        .arg(static_cast<int>(pos.y()))
        .arg(static_cast<int>(rotation)));
}

void EditorWindow::onModeCancelled() const {
    toolbar->setCheckpointMode(false);
    toolbar->setHintMode(false);
    updateStatusMessage("Modo cancelado - Navegación activada");
}

void EditorWindow::onHintDragUpdated(qreal angle) const {
    updateStatusMessage(QString("Dirección: %1°").arg(static_cast<int>(angle)));
}

void EditorWindow::onZoomIn() const {
    view->scale(1.2, 1.2);
    updateStatusMessage("Zoom aumentado");
}

void EditorWindow::onZoomOut() const {
    view->scale(1.0 / 1.2, 1.0 / 1.2);
    updateStatusMessage("Zoom reducido");
}

void EditorWindow::onResetZoom() const {
    view->resetTransform();
    updateStatusMessage("Zoom reseteado");
}

RaceTrackData EditorWindow::collectTrackData() const {
    RaceTrackData data;
    data.cityId = currentCity;

    for (const auto& cp : checkpointManager->getCheckpoints()) {
        CheckpointData cpData;
        cpData.position = cp.position;
        cpData.order = cp.order;
        data.checkpoints.push_back(cpData);
    }

    for (const auto& hint : hintManager->getHints()) {
        HintData hintData;
        hintData.position = hint.position;
        hintData.rotation = hint.rotation;
        data.hints.push_back(hintData);
    }

    return data;
}

void EditorWindow::restoreTrackData(const RaceTrackData& data) const {
    checkpointManager->clear();
    hintManager->clear();

    for (const auto& cpData : data.checkpoints) {
        checkpointManager->addCheckpoint(cpData.position);
    }

    for (const auto& hintData : data.hints) {
        hintManager->addHint(hintData.position, hintData.rotation);
    }

    inputHandler->setMode(EditorInputHandler::EditMode::NONE);
    toolbar->setCheckpointMode(false);
    toolbar->setHintMode(false);
}

bool EditorWindow::confirmDiscard() {
    if (checkpointManager->count() > 0 || hintManager->count() > 0) {
        const auto reply = QMessageBox::question(this, "Cargar Recorrido",
            "¿Descartar cambios actuales y cargar un recorrido?",
            QMessageBox::Yes | QMessageBox::No);
        return reply == QMessageBox::Yes;
    }
    return true;
}

bool EditorWindow::eventFilter(QObject* obj, QEvent* event) {
    if (obj != view->viewport()) {
        return QMainWindow::eventFilter(obj, event);
    }

    auto* mouseEvent = dynamic_cast<QMouseEvent*>(event);
    if (!mouseEvent) {
        return QMainWindow::eventFilter(obj, event);
    }

    bool handled = false;
    switch (event->type()) {
        case QEvent::MouseButtonPress:
            handled = inputHandler->handleMousePress(mouseEvent);
            break;
        case QEvent::MouseMove:
            handled = inputHandler->handleMouseMove(mouseEvent);
            break;
        case QEvent::MouseButtonRelease:
            handled = inputHandler->handleMouseRelease(mouseEvent);
            break;
        default:
            break;
    }

    return handled || QMainWindow::eventFilter(obj, event);
}