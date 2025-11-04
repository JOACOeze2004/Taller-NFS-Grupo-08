#ifndef TALLER_TP_HINT_MANAGER_H
#define TALLER_TP_HINT_MANAGER_H

#include <QGraphicsScene>
#include <QGraphicsPolygonItem>
#include <QPointF>
#include <QPolygonF>
#include <vector>

class HintManager {
public:
    struct Hint {
        QGraphicsPolygonItem* arrow{};
        QPointF position;
        qreal rotation{};
    };

    explicit HintManager(QGraphicsScene* scene);
    ~HintManager();

    void addHint(const QPointF& pos, qreal rotation);
    void removeLastHint();
    QGraphicsPolygonItem* createTempArrow(const QPointF& pos) const;
    void clear();
    const std::vector<Hint>& getHints() const { return hints; }
    size_t count() const { return hints.size(); }

private:
    QGraphicsScene* scene;
    std::vector<Hint> hints;
    
    static QPolygonF createArrowShape();
};

#endif // TALLER_TP_HINT_MANAGER_H