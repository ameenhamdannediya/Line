#include "MainWindow.h"
#include "TimelineView.h"

#include <QMenuBar>
#include <QSplitter>
#include <QFrame>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    resize(1000, 700);
    setWindowTitle("Line");

    // =========================
    // Menu Bar
    // =========================
    QMenu* fileMenu = menuBar()->addMenu("Line");
    fileMenu->addAction("New Line");
    fileMenu->addAction("Open Line");
    fileMenu->addSeparator();
    fileMenu->addAction("Exit", this, &QWidget::close);

    menuBar()->addMenu("Edit");
    menuBar()->addMenu("View");
    menuBar()->addMenu("Tools");
    menuBar()->addMenu("Help");

    // =========================
    // Central Splitter
    // =========================
    QSplitter* splitter = new QSplitter(Qt::Vertical, this);

    // --- VOID AREA (Node Editor) ---
    TimelineView* voidArea = new TimelineView(splitter);
    voidArea->setMinimumHeight(300);

    // --- DETAILS AREA (Bottom Panel) ---
    QFrame* detailsArea = new QFrame(splitter);
    detailsArea->setMinimumHeight(200);
    detailsArea->setStyleSheet("background-color: #222222;");

    splitter->addWidget(voidArea);
    splitter->addWidget(detailsArea);
    splitter->setSizes({ 500, 200 });

    // =========================
    // Set Central Widget ONCE
    // =========================
    setCentralWidget(splitter);
}
