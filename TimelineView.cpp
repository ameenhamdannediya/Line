#include "TimelineView.h"

#include <QGraphicsScene>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QGraphicsLineItem>
#include <QKeyEvent>




TimelineView::TimelineView(QWidget* parent)
    : QGraphicsView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setFocusPolicy(Qt::StrongFocus);

    setDragMode(QGraphicsView::NoDrag);


    QGraphicsScene* scene = new QGraphicsScene(this);
    setScene(scene);

    // Big scene for panning
    scene->setSceneRect(-5000, -500, 10000, 1000);

    // Timeline line
    QPen pen(Qt::white);
    pen.setWidth(2);
    m_timelineLine = scene->addLine(0, 0, 0, 0, pen);


    // View setup
    setRenderHint(QPainter::Antialiasing);
    setBackgroundBrush(QColor(17, 17, 17));
    setFrameStyle(QFrame::NoFrame);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
}


void TimelineView::wheelEvent(QWheelEvent* event)
{
    const double zoomFactor = 1.15;
    if (event->angleDelta().y() > 0)
        scale(zoomFactor, 1.0);
    else
        scale(1.0 / zoomFactor, 1.0);

    event->accept();
}

void TimelineView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton)
    {
        m_panning = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void TimelineView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_panning)
    {
        QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();

        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());

        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void TimelineView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton)
    {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}


void TimelineView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);

    QRectF viewRect = mapToScene(rect()).boundingRect();

    double leftMargin = viewRect.width() * 0.05;
    double rightMargin = viewRect.width() * 0.95;
    double centerY = viewRect.center().y();

    m_timelineLine->setLine(
        viewRect.left() + leftMargin,
        centerY,
        viewRect.left() + rightMargin,
        centerY
    );
}


void TimelineView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        resetTransform();
        centerOn(0, 0);
        event->accept();
        return;
    }
    QGraphicsView::keyPressEvent(event);
}
