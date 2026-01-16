#include "TimelineView.h"
#include "NodeItem.h"
#include "ConnectionItem.h"
#include "Shortcuts.h"
#include "BlockItem.h"

#include <QMenu>
#include <QKeyEvent>
#include <QScrollBar>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QtMath>  // for qFloor
#include <QPainter>
#include <QStyleOptionGraphicsItem>




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
    BlockItem* clickedBlock = dynamic_cast<BlockItem*>(item);


    // ---- Middle mouse pan ----
    if (event->button() == Qt::MiddleButton) {
        panning = true;
        lastPanPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

    // ---- LEFT CLICK ----
    if (event->button() == Qt::LeftButton) {

        // ---- Ctrl + Left click (create / split) ----
        if (event->modifiers() & Qt::ControlModifier) {

            QList<QGraphicsItem*> selected = scene->selectedItems();

            // Split edge
            if (selected.size() == 1) {
                if (auto* edge = dynamic_cast<ConnectionItem*>(selected.first())) {
                    splitSelectedEdge(scenePos);
                    event->accept();
                    return;
                }
            }

            // Create node
            NodeItem* newNode = new NodeItem();
            newNode->setPos(scenePos);
            scene->addItem(newNode);

            for (QGraphicsItem* sel : selected) {
                if (auto* selNode = dynamic_cast<NodeItem*>(sel)) {
                    scene->addItem(new ConnectionItem(selNode, newNode));
                }
            }

            scene->clearSelection();
            newNode->setSelected(true);
            event->accept();
            return;
        }

        // ---- Shift + Left click (multi-select toggle) ----
        if (event->modifiers() & Qt::ShiftModifier) {
            if (item)
                item->setSelected(!item->isSelected());
            event->accept();
            return;
        }

        // ---- Left click on node: start edge drag ----
        if (clickedNode) {
            scene->clearSelection();
            clickedNode->setSelected(true);

            activeNode = clickedNode;
            tempConnection = new ConnectionItem(activeNode);
            scene->addItem(tempConnection);
            tempConnection->setEndPosition(scenePos);
            event->accept();
            return;
        }
        // ---- Left click on block: select it ----
        if (clickedBlock) {
            scene->clearSelection();
            clickedBlock->setSelected(true);
            event->accept();
            return;
        }

        // ---- Left click on empty space: clear selection ----
        scene->clearSelection();
        event->accept();
        return;
    }

    QGraphicsView::mousePressEvent(event);
}



void TimelineView::mouseMoveEvent(QMouseEvent* event)
{
    QPointF scenePos = mapToScene(event->pos());

    // Move nodes in grab mode
    if (grabbing) {
        QPointF currentMouseScenePos = mapToScene(event->pos());
        QPointF delta = currentMouseScenePos - grabStartMouseScenePos;

        for (auto it = grabStartPositions.begin(); it != grabStartPositions.end(); ++it)
            it.key()->setPos(it.value() + delta);

        event->accept();
        return;
    }



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


    

    // Update connection preview
    if (tempConnection) {
        tempConnection->setEndPosition(scenePos);
        return;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void TimelineView::mouseReleaseEvent(QMouseEvent* event)
{

    if (grabbing && event->button() == Qt::LeftButton) {
        grabbing = false;
        grabStartPositions.clear();
        event->accept();
        return;
    }

    // Right click cancels grab
    if (grabbing && event->button() == Qt::RightButton) {
        for (auto it = grabStartPositions.begin(); it != grabStartPositions.end(); ++it)
            it.key()->setPos(it.value());

        grabbing = false;
        grabStartPositions.clear();
        event->accept();
        return;
    }


   /* if (movingNode) {
        movingNode = false;
        activeNode = nullptr;
        return;
    }*/

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
            // CLEAN CANCEL: detach from start node
            activeNode->removeConnection(tempConnection);

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
    //shift A
    if (event->key() == Qt::Key_A &&
        (event->modifiers() & Qt::ShiftModifier))
    {
        QMenu menu;
        QMenu* blocksMenu = menu.addMenu("Blocks");

        QAction* eventBlockAction = blocksMenu->addAction("Event");

        QAction* selected = menu.exec(QCursor::pos());

        if (selected == eventBlockAction) {
            QPointF scenePos = mapToScene(mapFromGlobal(QCursor::pos()));

            BlockItem* block = new BlockItem("Event");
            block->setPos(scenePos);
            scene->addItem(block);
        }

        event->accept();
        return;
    }


    // ---- Press G to grab selected nodes ----
    if (event->key() == Qt::Key_G &&
        !grabbing &&
        !scene->selectedItems().isEmpty()) {

        grabbing = true;
        grabStartMouseScenePos = mapToScene(mapFromGlobal(QCursor::pos()));
        grabStartPositions.clear();

        // Save original positions
        for (QGraphicsItem* item : scene->selectedItems()) {
            grabStartPositions[item] = item->pos();
        }


        event->accept();
        return;
    }

    // ---- Cancel grab ----
    if (grabbing && (event->key() == Qt::Key_Escape)) {
        // Reset positions
        for (auto it = grabStartPositions.begin(); it != grabStartPositions.end(); ++it)
            it.key()->setPos(it.value());

        grabbing = false;
        grabStartPositions.clear();
        event->accept();
        return;
    }


   


    if (event->key() == Qt::Key_Backspace) {

        QList<ConnectionItem*> edgesToDelete;
        QList<NodeItem*> nodesToDelete;
        QList<BlockItem*> blocksToDelete;

        for (QGraphicsItem* item : scene->selectedItems()) {
            if (auto* edge = dynamic_cast<ConnectionItem*>(item))
                edgesToDelete.append(edge);
            else if (auto* node = dynamic_cast<NodeItem*>(item))
                nodesToDelete.append(node);
            else if (auto* block = dynamic_cast<BlockItem*>(item))
                blocksToDelete.append(block);
        }

        // ---- Delete edges FIRST ----
        for (ConnectionItem* edge : edgesToDelete) {
            if (edge->scene())
                edge->scene()->removeItem(edge);
            delete edge;
        }

        // ---- Delete nodes SECOND ----
        for (NodeItem* node : nodesToDelete) {
            QSet<ConnectionItem*> edges = node->getAllConnections();
            for (ConnectionItem* edge : edges) {
                if (edge->scene())
                    edge->scene()->removeItem(edge);
                delete edge;
            }

            if (node->scene())
                node->scene()->removeItem(node);

            delete node;
        }

        // ---- Delete blocks LAST ----
        for (BlockItem* block : blocksToDelete) {
            if (block->scene())
                block->scene()->removeItem(block);
            delete block;
        }

        event->accept();
        return;
    }



   
    // select all (A key)
    if (event->key() == Qt::Key_A &&
        !(event->modifiers() & (Qt::ControlModifier |
            Qt::AltModifier |
            Qt::ShiftModifier))) {

        scene->clearSelection();
        for (QGraphicsItem* item : scene->items())
            item->setSelected(true);

        event->accept();
        return;
    }


    



    // --- New addition: Esc = frame all ---
    if (event->key() == Qt::Key_Escape) {
        frameAll();
        event->accept();
        return;
    }

    // For all other keys, call base
    QGraphicsView::keyPressEvent(event);
}

void TimelineView::splitSelectedEdge(const QPointF& scenePos)
{
    QList<QGraphicsItem*> selected = scene->selectedItems();

    if (selected.size() != 1)
        return;

    ConnectionItem* edge = dynamic_cast<ConnectionItem*>(selected.first());
    if (!edge)
        return;

    auto* start = dynamic_cast<NodeItem*>(edge->getStartItem());
    auto* end = dynamic_cast<NodeItem*>(edge->getEndItem());

    if (!start || !end)
        return; // only split node-to-node edges

    NodeItem* newNode = new NodeItem();
    newNode->setPos(scenePos);
    scene->addItem(newNode);

    start->removeConnection(edge);
    end->removeConnection(edge);

    if (edge->scene())
        edge->scene()->removeItem(edge);

    delete edge;

    scene->addItem(new ConnectionItem(start, newNode));
    scene->addItem(new ConnectionItem(newNode, end));

    scene->clearSelection();
    newNode->setSelected(true);
}












