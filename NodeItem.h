#pragma once
#include <QGraphicsItem>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QDate>


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


    //details
    void setTitle(const QString& t) { title = t.isEmpty() ? "Event" : t; }
    QString getTitle() const { return title; }

    void setDescription(const QString& d) { description = d; }
    QString getDescription() const { return description; }

    void addAttachment(const QString& path) { attachments.append(path); }
    QStringList getAttachments() const { return attachments; }

    void setDate(const QDate& d) { date = d; }
    QDate getDate() const { return date; }

    int getId() const { return id; }
    void setId(int i) { id = i; }


protected:
    QVariant itemChange(GraphicsItemChange change,
        const QVariant& value) override;

private:
    QSet<ConnectionItem*> connections;
    QDate date = QDate::currentDate();

    QString title = "Event";
    QString description;
    QStringList attachments;

    int id = -1;

};
