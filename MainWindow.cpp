#include "MainWindow.h"
#include "TimelineView.h"


#include <QMenuBar>
#include <QWidget>

#include <QSplitter>
#include <QFrame>
#include <QVBoxLayout>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    resize(1000, 700);
    setWindowTitle("Line");

    // Menu bar
    QMenu* fileMenu = menuBar()->addMenu("Line");
    fileMenu->addAction("New Line");
    fileMenu->addAction("Open Line");
    fileMenu->addSeparator();
    fileMenu->addAction("Exit", this, &QWidget::close);

    menuBar()->addMenu("Edit");
    menuBar()->addMenu("View");
    menuBar()->addMenu("Tools");
    menuBar()->addMenu("Help");

    // Central placeholder
    // Splitter (top = void, bottom = details)
    QSplitter* splitter = new QSplitter(Qt::Vertical, this);

   
    // --- VOID (timeline) ---
    TimelineView* voidArea = new TimelineView(splitter);
    voidArea->setMinimumHeight(300);


    // --- DETAILS (bottom) ---
    QFrame* detailsArea = new QFrame(splitter);
    detailsArea->setStyleSheet("background-color: #222222;");
    detailsArea->setMinimumHeight(200);


    // Add to splitter
    splitter->addWidget(voidArea);
    splitter->addWidget(detailsArea);

    // Set initial sizes
    splitter->setSizes({ 500, 200 });

    // Set as central widget
    setCentralWidget(splitter);

}
