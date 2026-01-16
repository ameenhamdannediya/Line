#pragma once
#include <QGraphicsPathItem>

class QGraphicsItem;

class ConnectionItem : public QGraphicsPathItem
{
public:
    ConnectionItem(QGraphicsItem* start, QGraphicsItem* end = nullptr);

    void setEndPosition(const QPointF& pos);
    void finalize(QGraphicsItem* endItem);
    virtual void updatePosition();

    void paint(QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr) override;

    QGraphicsItem* getStartItem() const { return startItem; }
    QGraphicsItem* getEndItem() const { return endItem; }

private:
    QGraphicsItem* startItem;
    QGraphicsItem* endItem;

    QPointF tempEndPos;
    qreal curvature = 80.0;
};
