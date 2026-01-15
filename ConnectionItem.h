#pragma once
#include <QGraphicsPathItem>

class NodeItem;

class ConnectionItem : public QGraphicsPathItem
{
public:
    ConnectionItem(NodeItem* start, NodeItem* end = nullptr);

    void setEndPosition(const QPointF& pos);
    void finalize(NodeItem* endNode);
    void updatePosition();
    void paint(QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr) override;

    NodeItem* getStartNode() const { return startNode; }
    NodeItem* getEndNode() const { return endNode; }

private:
    NodeItem* startNode;
    NodeItem* endNode = nullptr;
    QPointF tempEndPos;
    qreal curvature = 80.0;
};
