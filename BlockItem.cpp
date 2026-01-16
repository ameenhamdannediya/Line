#include "BlockItem.h"
#include "PortItem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

BlockItem::BlockItem(const QString& t)
    : title(t)
{
    setFlags(ItemIsSelectable | ItemIsMovable | ItemSendsGeometryChanges);

    inputPort = new PortItem(PortType::Input, this);
    outputPort = new PortItem(PortType::Output, this);

    inputPort->setPos(13, 40);
    outputPort->setPos(width - 7, 40);
}


QRectF BlockItem::boundingRect() const
{
    return QRectF(0, 0, width, height);
}

void BlockItem::paint(QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);

    QColor bg(45, 45, 45);
    QColor border(120, 120, 120);

    if (option->state & QStyle::State_Selected)
        border = QColor(80, 160, 255);

    // Body
    painter->setBrush(bg);
    painter->setPen(QPen(border, 2));
    painter->drawRoundedRect(boundingRect(), radius, radius);

    // Title bar
    QRectF titleBar(0, 0, width, 26);
    painter->setBrush(QColor(60, 60, 60));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(titleBar, radius, radius);
    painter->drawRect(0, 13, width, 13); // flatten bottom

    // Title text
    painter->setPen(Qt::white);
    painter->drawText(titleBar.adjusted(8, 0, -8, 0),
        Qt::AlignVCenter | Qt::AlignLeft,
        title);



    // Ports
    QColor portColor(200, 200, 200);
    painter->setBrush(portColor);
    painter->setPen(Qt::NoPen);

    qreal r = 4;





}

