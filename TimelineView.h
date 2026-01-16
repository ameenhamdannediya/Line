#pragma once
#include <QGraphicsView>

class QGraphicsScene;
class NodeItem;
class ConnectionItem;


class TimelineView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TimelineView(QWidget* parent = nullptr);

    void drawBackground(QPainter* painter, const QRectF& rect);


protected:
    void wheelEvent(QWheelEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void keyPressEvent(QKeyEvent*) override;

private:
    void splitSelectedEdge(const QPointF& scenePos);
    void frameAll();


    bool panning = false;
    QPoint lastPanPos;

    QGraphicsScene* scene;

    NodeItem* activeNode = nullptr;
    ConnectionItem* tempConnection = nullptr;

    bool movingNode = false;
    QPointF lastMouseScenePos;



    bool grabbing = false;              // true when in “G grab” mode
    QPointF grabStartMouseScenePos;     // original mouse position when G pressed
    QMap<QGraphicsItem*, QPointF> grabStartPositions;






};
