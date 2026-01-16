#pragma once
#include <QGraphicsItem>
#include <QString>

class PortItem;   // forward declaration only

class BlockItem : public QGraphicsItem
{
public:
    BlockItem(const QString& title = "Block");

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr) override;

    PortItem* getInputPort() const { return inputPort; }
    PortItem* getOutputPort() const { return outputPort; }

private:
    QString title;
    qreal width = 160;
    qreal height = 80;
    qreal radius = 8;

    PortItem* inputPort;
    PortItem* outputPort;
};
