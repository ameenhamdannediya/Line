#include "ConnectionItem.h"
#include <QPainter>

ConnectionItem::ConnectionItem(QGraphicsItem* start, QGraphicsItem* end)
    : startItem(start), endItem(end)
{
    setZValue(-1);
    setFlag(QGraphicsItem::ItemIsSelectable);
    updatePosition();
}

void ConnectionItem::setEndPosition(const QPointF& pos)
{
    tempEndPos = pos;
    updatePosition();
}

void ConnectionItem::finalize(QGraphicsItem* end)
{
    endItem = end;
    updatePosition();
}

void ConnectionItem::updatePosition()
{
    if (!startItem)
        return;

    QPointF p1 = startItem->scenePos();
    QPointF p2 = endItem ? endItem->scenePos() : tempEndPos;

    QPointF c1 = p1 + QPointF(curvature, 0);
    QPointF c2 = p2 - QPointF(curvature, 0);

    QPainterPath path(p1);
    path.cubicTo(c1, c2, p2);
    setPath(path);
}

void ConnectionItem::paint(QPainter* painter,
    const QStyleOptionGraphicsItem*,
    QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen;
    if (isSelected()) {
        pen.setColor(QColor(80, 160, 255));
        pen.setWidthF(3.0);
    }
    else {
        pen.setColor(QColor(180, 180, 180));
        pen.setWidthF(1.5);
    }

    painter->setPen(pen);
    painter->drawPath(path());
}
