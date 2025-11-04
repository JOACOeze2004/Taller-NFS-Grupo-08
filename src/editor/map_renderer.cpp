#include "map_renderer.h"
#include <QFile>
#include <QDebug>

MapRenderer::MapRenderer(QGraphicsScene* scene)
    : scene(scene), backgroundItem(nullptr) {}

MapRenderer::~MapRenderer() {
    clear();
}

QString MapRenderer::getMapPath(const QString& cityId) {
    const QString basePath = "../assets/need-for-speed/cities/";

    if (cityId == "liberty_city") {
        return basePath + "Game Boy _ GBC - Grand Theft Auto - Backgrounds - Liberty City.png";
    }
    if (cityId == "san_andreas") {
        return basePath + "Game Boy _ GBC - Grand Theft Auto - Backgrounds - San Andreas.png";
    }
    if (cityId == "vice_city") {
        return basePath + "Game Boy _ GBC - Grand Theft Auto - Backgrounds - Vice City.png";
    }
    return "";
}

bool MapRenderer::loadCityMap(const QString& cityId) {
    clear();

    const QString mapPath = getMapPath(cityId);

    if (!QFile::exists(mapPath)) {
        qCritical() << "ERROR: Imagen de mapa no encontrada:" << mapPath;
        qCritical() << "El editor requiere que existan las imágenes de los mapas.";
        return false;
    }

    QPixmap mapImage;
    if (!mapImage.load(mapPath)) {
        qCritical() << "ERROR: No se pudo cargar la imagen:" << mapPath;
        return false;
    }

    backgroundItem = scene->addPixmap(mapImage);
    backgroundItem->setZValue(-1);
    scene->setSceneRect(mapImage.rect());

    return true;
}

QSizeF MapRenderer::getMapSize() const {
    if (backgroundItem) {
        return backgroundItem->boundingRect().size();
    }
    return QSizeF(0, 0);
}

void MapRenderer::clear() {
    if (backgroundItem) {
        scene->removeItem(backgroundItem);
        delete backgroundItem;
        backgroundItem = nullptr;
    }
}