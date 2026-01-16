#include "PortItem.h"
#include <QPainter>

PortItem::PortItem(PortType t, QGraphicsItem* parent)
    : QGraphicsItem(parent), type(t)
{
    setFlag(ItemIsSelectable);
    setAcceptHoverEvents(true);
}


QRectF PortItem::boundingRect() const
{
    return QRectF(-radius, -radius, radius * 2, radius * 2);
}

void PortItem::paint(QPainter* painter,
    const QStyleOptionGraphicsItem*,
    QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);

    QColor c = (type == PortType::Input)
        ? QColor(120, 200, 120)
        : QColor(120, 120, 200);

    painter->setBrush(c);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(boundingRect());
}
QPointF PortItem::getConnectionPos() const
{
    return scenePos();
}

