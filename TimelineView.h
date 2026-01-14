#pragma once

#include <QGraphicsView>

class TimelineView : public QGraphicsView
{
    Q_OBJECT

public:

    explicit TimelineView(QWidget* parent = nullptr);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;

private:
    bool m_panning = false;
    QPoint m_lastMousePos;

    QGraphicsLineItem* m_timelineLine = nullptr;
    void keyPressEvent(QKeyEvent* event) override;




};
