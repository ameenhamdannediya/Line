#include "StickConnectionItem.h"
#include "NodeItem.h"

#include <QPainter>
#include <QPainterPath>

StickConnectionItem::StickConnectionItem(NodeItem* start, NodeItem* end)
    : ConnectionItem(start, end)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
}

void StickConnectionItem::updatePosition()
{
    QPointF p1 = getStartNode()->scenePos();
    QPointF p2 = getEndNode()->scenePos();

    QPainterPath path(p1);
    path.lineTo(p2); // ALWAYS STRAIGHT
    setPath(path);
}

// ================== THIS IS THE IMPORTANT PART ==================
void StickConnectionItem::paint(QPainter* painter,
    const QStyleOptionGraphicsItem*,
    QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setColor(QColor(200, 200, 200));
    pen.setWidthF(isSelected() ? 4.0 : 3.0); // thicker than normal

    painter->setPen(pen);
    painter->drawPath(path());
}
