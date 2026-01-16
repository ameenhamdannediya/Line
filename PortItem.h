#pragma once
#include <QGraphicsItem>

enum class PortType {
    Input,
    Output
};

class PortItem : public QGraphicsItem
{
public:

    PortItem(PortType type, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr) override;

    PortType getType() const { return type; }

    QPointF getConnectionPos() const;


private:
    PortType type;
    qreal radius = 5;
};
