#include "NodeItem.h"
#include "ConnectionItem.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QStyle>

NodeItem::NodeItem()
{
    setFlags(ItemIsSelectable | ItemSendsGeometryChanges);
}

NodeItem::~NodeItem()
{
    // do NOT delete edges here; TimelineView will handle
}

QRectF NodeItem::boundingRect() const
{
    return QRectF(-5, -5, 10, 10);
}

void NodeItem::paint(QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);
    QColor fill = Qt::white;
    QColor outline = Qt::NoPen;
    if (option->state & QStyle::State_Selected)
        outline = QColor(80, 160, 255); // blue outline
    painter->setBrush(fill);
    painter->setPen(QPen(outline, 2));
    painter->drawEllipse(boundingRect());
}

void NodeItem::addConnection(ConnectionItem* c)
{
    connections.insert(c);
}

void NodeItem::removeConnection(ConnectionItem* c)
{
    connections.remove(c);
}

QVariant NodeItem::itemChange(GraphicsItemChange change,
    const QVariant& value)
{
    if (change == ItemPositionHasChanged) {
        for (ConnectionItem* c : connections)
            if (c) c->updatePosition();
    }
    return QGraphicsItem::itemChange(change, value);
}
QPointF NodeItem::getConnectionPos() const
{
    return scenePos();
}
