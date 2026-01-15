#include "ConnectionItem.h"
#include "NodeItem.h"
#include <QPainter>

ConnectionItem::ConnectionItem(NodeItem* start, NodeItem* end)
    : startNode(start), endNode(end)
{
    setZValue(-1);
    setFlag(QGraphicsItem::ItemIsSelectable);
    startNode->addConnection(this);
    if (endNode) endNode->addConnection(this);
    updatePosition();
}

void ConnectionItem::setEndPosition(const QPointF& pos)
{
    tempEndPos = pos;
    updatePosition();
}

void ConnectionItem::finalize(NodeItem* end)
{
    endNode = end;
    endNode->addConnection(this);
    updatePosition();
}

void ConnectionItem::updatePosition()
{
    QPointF p1 = startNode->scenePos();
    QPointF p2 = endNode ? endNode->scenePos() : tempEndPos;

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
