#ifndef TALLER_TP_MAP_RENDERER_H
#define TALLER_TP_MAP_RENDERER_H

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QString>
#include <QPixmap>

class MapRenderer {
public:
    explicit MapRenderer(QGraphicsScene* scene);
    ~MapRenderer();
    bool loadCityMap(const QString& cityId);
    QSizeF getMapSize() const;
    void clear();

private:
    QGraphicsScene* scene;
    QGraphicsPixmapItem* backgroundItem;
    
    static QString getMapPath(const QString& cityId) ;
};

#endif // TALLER_TP_MAP_RENDERER_H