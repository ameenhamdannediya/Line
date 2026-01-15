#pragma once
#include <QGraphicsItem>
#include <QSet>

class ConnectionItem;

class NodeItem : public QGraphicsItem
{
public:
    NodeItem();
    ~NodeItem();  // keep empty, do NOT delete edges here

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr) override;

    void addConnection(ConnectionItem* connection);
    void removeConnection(ConnectionItem* connection);

    // Safe access for deletion
    QSet<ConnectionItem*> getAllConnections() { return connections; }

protected:
    QVariant itemChange(GraphicsItemChange change,
        const QVariant& value) override;

private:
    QSet<ConnectionItem*> connections;
};
