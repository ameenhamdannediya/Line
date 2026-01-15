#include "TimelineView.h"
#include "NodeItem.h"
#include "ConnectionItem.h"
#include <QKeyEvent>
#include <QScrollBar>

#include <QGraphicsScene>
#include <QMouseEvent>

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtMath>  // for qFloor


TimelineView::TimelineView(QWidget* parent)
    : QGraphicsView(parent),
    scene(new QGraphicsScene(this))
{

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setFocusPolicy(Qt::StrongFocus);


    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::NoDrag);
    setBackgroundBrush(QColor(30, 30, 30));  // dark gray (Blender-like)
    scene->setSceneRect(-5000, -5000, 10000, 10000);
}




void TimelineView::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->fillRect(rect, QColor(30, 30, 30)); // dark background

    const qreal gridSize = 50;   // minor grid
    const qreal majorGridSize = 250; // major grid
    const QColor minorColor(50, 50, 50);
    const QColor majorColor(80, 80, 80);

    QRectF visibleRect = mapToScene(viewport()->rect()).boundingRect();

    qreal left = qFloor(visibleRect.left() / gridSize) * gridSize;
    qreal right = qCeil(visibleRect.right() / gridSize) * gridSize;
    qreal top = qFloor(visibleRect.top() / gridSize) * gridSize;
    qreal bottom = qCeil(visibleRect.bottom() / gridSize) * gridSize;

    // Draw minor grid
    painter->setPen(minorColor);
    for (qreal x = left; x <= right; x += gridSize)
        painter->drawLine(QPointF(x, top), QPointF(x, bottom));
    for (qreal y = top; y <= bottom; y += gridSize)
        painter->drawLine(QPointF(left, y), QPointF(right, y));

    // Draw major grid
    left = qFloor(visibleRect.left() / majorGridSize) * majorGridSize;
    right = qCeil(visibleRect.right() / majorGridSize) * majorGridSize;
    top = qFloor(visibleRect.top() / majorGridSize) * majorGridSize;
    bottom = qCeil(visibleRect.bottom() / majorGridSize) * majorGridSize;

    painter->setPen(QPen(majorColor, 2));
    for (qreal x = left; x <= right; x += majorGridSize)
        painter->drawLine(QPointF(x, top), QPointF(x, bottom));
    for (qreal y = top; y <= bottom; y += majorGridSize)
        painter->drawLine(QPointF(left, y), QPointF(right, y));
}






void TimelineView::mousePressEvent(QMouseEvent* event)
{
    QPointF scenePos = mapToScene(event->pos());
    QGraphicsItem* item = itemAt(event->pos());
    NodeItem* clickedNode = dynamic_cast<NodeItem*>(item);

    // --- Middle mouse pan ---
    if (event->button() == Qt::MiddleButton) {
        panning = true;
        lastPanPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

    // --- Shift + Right click: Multi-selection ---
    if (event->button() == Qt::RightButton && (event->modifiers() & Qt::ShiftModifier)) {
        if (item) {
            item->setSelected(!item->isSelected()); // toggle selection
        }
        event->accept();
        return;
    }

    // --- Right click (no shift): select single item ---
    if (event->button() == Qt::RightButton) {
        scene->clearSelection();
        if (item)
            item->setSelected(true);
        event->accept();
        return;
    }

    // --- Shift + Drag = Move node ---
    if (clickedNode && (event->modifiers() & Qt::ShiftModifier)) {
        movingNode = true;
        activeNode = clickedNode;
        lastMouseScenePos = scenePos;
        event->accept();
        return;
    }

    // --- Left click on empty space ---
    if (!clickedNode && event->button() == Qt::LeftButton) {
        NodeItem* newNode = new NodeItem();
        newNode->setPos(scenePos);
        scene->addItem(newNode);

        // --- Auto-connect from all currently selected nodes ---
        for (QGraphicsItem* selItem : scene->selectedItems()) {
            NodeItem* selNode = dynamic_cast<NodeItem*>(selItem);
            if (selNode)
                scene->addItem(new ConnectionItem(selNode, newNode));
        }

        // Optional: select new node after creation
        scene->clearSelection();
        newNode->setSelected(true);

        event->accept();
        return;
    }

    // --- Left click on node = start drag-to-connect ---
    if (clickedNode && event->button() == Qt::LeftButton) {
        activeNode = clickedNode;
        tempConnection = new ConnectionItem(activeNode);
        scene->addItem(tempConnection);
        tempConnection->setEndPosition(scenePos);
        event->accept();
        return;
    }

    QGraphicsView::mousePressEvent(event);
}


void TimelineView::mouseMoveEvent(QMouseEvent* event)
{
    QPointF scenePos = mapToScene(event->pos());

    if (panning) {
        QPoint delta = event->pos() - lastPanPos;
        lastPanPos = event->pos();

        horizontalScrollBar()->setValue(
            horizontalScrollBar()->value() - delta.x());

        verticalScrollBar()->setValue(
            verticalScrollBar()->value() - delta.y());

        event->accept();
        return;
    }


    // Move node (Shift held)
    if (movingNode && activeNode) {
        QPointF delta = scenePos - lastMouseScenePos;
        activeNode->setPos(activeNode->pos() + delta);
        lastMouseScenePos = scenePos;
        return;
    }

    // Update connection preview
    if (tempConnection) {
        tempConnection->setEndPosition(scenePos);
        return;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void TimelineView::mouseReleaseEvent(QMouseEvent* event)
{
    if (movingNode) {
        movingNode = false;
        activeNode = nullptr;
        return;
    }

    if (event->button() == Qt::MiddleButton) {
        panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }



    if (tempConnection && activeNode) {
        QGraphicsItem* item = itemAt(event->pos());
        NodeItem* endNode = dynamic_cast<NodeItem*>(item);

        // Prevent self-connection
        if (endNode && endNode != activeNode) {
            tempConnection->finalize(endNode);
        }
        else {
            scene->removeItem(tempConnection);
            delete tempConnection;
        }
    }

    tempConnection = nullptr;
    activeNode = nullptr;

    QGraphicsView::mouseReleaseEvent(event);
}


void TimelineView::wheelEvent(QWheelEvent* event)
{
    constexpr qreal zoomFactor = 1.15;
    constexpr qreal minScale = 0.2;
    constexpr qreal maxScale = 5.0;

    qreal currentScale = transform().m11();

    if (event->angleDelta().y() > 0 && currentScale < maxScale)
        scale(zoomFactor, zoomFactor);
    else if (event->angleDelta().y() < 0 && currentScale > minScale)
        scale(1.0 / zoomFactor, 1.0 / zoomFactor);

    event->accept();
}


void TimelineView::frameAll()
{
    if (scene->items().isEmpty())
        return;

    // Get bounding rect of all items
    QRectF bounds = scene->itemsBoundingRect();

    // Add margin (Blender-like padding)
    constexpr qreal padding = 50.0;
    bounds.adjust(-padding, -padding, padding, padding);

    // Fit view to bounds
    fitInView(bounds, Qt::KeepAspectRatio);
}



void TimelineView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() != Qt::Key_Backspace) {
        QGraphicsView::keyPressEvent(event);
        return;
    }

    auto itemsToDelete = scene->selectedItems();
    if (itemsToDelete.isEmpty()) return;

    // Step 1: Delete selected edges first
    for (QGraphicsItem* item : itemsToDelete) {
        if (ConnectionItem* edge = dynamic_cast<ConnectionItem*>(item)) {
            NodeItem* start = edge->getStartNode();
            NodeItem* end = edge->getEndNode();
            if (start) start->removeConnection(edge);
            if (end) end->removeConnection(edge);
            if (edge->scene()) edge->scene()->removeItem(edge);
            delete edge;
        }
    }

    // Step 2: Delete selected nodes
    for (QGraphicsItem* item : itemsToDelete) {
        if (NodeItem* node = dynamic_cast<NodeItem*>(item)) {
            // Delete all edges connected to node (safe even if node has none)
            QSet<ConnectionItem*> edges = node->getAllConnections();
            for (ConnectionItem* edge : edges) {
                NodeItem* other = (edge->getStartNode() == node) ? edge->getEndNode() : edge->getStartNode();
                if (other) other->removeConnection(edge);
                if (edge->scene()) edge->scene()->removeItem(edge);
                delete edge;
            }

            // Now delete the node itself
            if (node->scene()) node->scene()->removeItem(node);
            delete node;
        }
    }

    event->accept();
}






