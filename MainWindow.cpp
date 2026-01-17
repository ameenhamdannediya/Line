#include "MainWindow.h"
#include "TimelineView.h"
#include "NodeItem.h"
#include "ConnectionItem.h"
#include "Shortcuts.h"

#include <QKeyEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QMenuBar>
#include <QSplitter>
#include <QFrame>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QPushButton>
#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    resize(1000, 700);
    setWindowTitle("Line");

    // =========================
    // Menu Bar
    // =========================
    QMenu* fileMenu = menuBar()->addMenu("Line");

    QAction* newAct = fileMenu->addAction("New");
    newAct->setShortcut(Shortcuts::NewFile);

    QAction* openAct = fileMenu->addAction("Open...");
    openAct->setShortcut(Shortcuts::Open);

    QAction* saveAct = fileMenu->addAction("Save");
    saveAct->setShortcut(Shortcuts::Save);

    QAction* saveAsAct = fileMenu->addAction("Save As...");
    saveAsAct->setShortcut(Shortcuts::SaveAs);

    fileMenu->addSeparator();

    QAction* exitAct = fileMenu->addAction("Exit");
    exitAct->setShortcut(Shortcuts::QuitSeq);




    menuBar()->addMenu("Edit");
    menuBar()->addMenu("View");
    menuBar()->addMenu("Tools");
    menuBar()->addMenu("Help");

    // =========================
    // Central Splitter
    // =========================
    QSplitter* splitter = new QSplitter(Qt::Vertical, this);

    // --- VOID AREA (Node Editor) ---
    timeline = new TimelineView(splitter);
    timeline->setMinimumHeight(300);

    // --- DETAILS AREA ---
    QWidget* detailsArea = new QWidget(splitter);
    detailsArea->setMinimumHeight(200);
    detailsArea->setStyleSheet("background:#222;color:white;");

    QHBoxLayout* mainLayout = new QHBoxLayout(detailsArea);

    // ================= LEFT =================
    QVBoxLayout* leftLayout = new QVBoxLayout();

    leftLayout->addWidget(new QLabel("Title"));
    titleEdit = new QLineEdit();
    titleEdit->setPlaceholderText("Event");
    leftLayout->addWidget(titleEdit);

    leftLayout->addWidget(new QLabel("Date"));
    dateEdit = new QDateEdit();
    dateEdit->setCalendarPopup(true);
    dateEdit->setDate(QDate::currentDate());
    leftLayout->addWidget(dateEdit);

    leftLayout->addWidget(new QLabel("Description"));
    descriptionEdit = new QTextEdit();
    leftLayout->addWidget(descriptionEdit);

    // ================= RIGHT =================
    QVBoxLayout* rightLayout = new QVBoxLayout();

    rightLayout->addWidget(new QLabel("Attachments"));
    attachmentsList = new QListWidget();
    rightLayout->addWidget(attachmentsList);

    QPushButton* attachBtn = new QPushButton("Add File");
    rightLayout->addWidget(attachBtn);

    // ================= MERGE =================
    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 1);


    splitter->addWidget(timeline);
    splitter->addWidget(detailsArea);
    splitter->setSizes({ 200, 500 });

    setCentralWidget(splitter);

    // =========================
    // Connections
    // =========================
    connect(timeline, &TimelineView::nodeSelected,
        this, &MainWindow::onNodeSelected);

    connect(titleEdit, &QLineEdit::textChanged, this, [=](const QString& t) {
        if (currentNode)
            currentNode->setTitle(t);
        });

    connect(descriptionEdit, &QTextEdit::textChanged, this, [=]() {
        if (currentNode)
            currentNode->setDescription(descriptionEdit->toPlainText());
        });

    connect(attachBtn, &QPushButton::clicked, this, [=]() {
        if (!currentNode) return;

        QString file = QFileDialog::getOpenFileName(this, "Attach File");
        if (!file.isEmpty()) {
            currentNode->addAttachment(file);
            attachmentsList->addItem(file);
        }
        });
    connect(dateEdit, &QDateEdit::dateChanged, this, [=](const QDate& d) {
        if (currentNode)
            currentNode->setDate(d);
        });


    connect(newAct, &QAction::triggered, this, &MainWindow::newProject);
    connect(openAct, &QAction::triggered, this, &MainWindow::loadProject);
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveProject);
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveProject);
    connect(exitAct, &QAction::triggered, this, &QWidget::close);


}

void MainWindow::onNodeSelected(NodeItem* node)
{
    currentNode = node;

    if (!node) {
        titleEdit->clear();
        descriptionEdit->clear();
        attachmentsList->clear();
        return;
    }

    titleEdit->setText(node->getTitle());
    dateEdit->setDate(node->getDate());
    descriptionEdit->setPlainText(node->getDescription());


    attachmentsList->clear();
    for (auto& a : node->getAttachments())
        attachmentsList->addItem(a);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->key() == Qt::Key_S) {
            saveProject();
            return;
        }
        if (event->key() == Qt::Key_O) {
            loadProject();
            return;
        }
    }

    QMainWindow::keyPressEvent(event);
}


//save and load 
void MainWindow::saveProject()
{
    QString path = QFileDialog::getSaveFileName(
        this, "Save Project", "", "Line Files (*.line)");

    if (path.isEmpty()) return;
    if (!path.endsWith(".line"))
        path += ".line";

    QJsonObject root;
    QJsonArray nodesArray;
    QJsonArray edgesArray;

    QMap<NodeItem*, int> idMap;
    int idCounter = 0;

    // ---- Save nodes ----
    for (QGraphicsItem* item : timeline->getScene()->items()) {
        if (auto* node = dynamic_cast<NodeItem*>(item)) {
            int id = idCounter++;
            node->setId(id);
            idMap[node] = id;

            QJsonObject n;
            n["id"] = id;
            n["x"] = node->pos().x();
            n["y"] = node->pos().y();
            n["title"] = node->getTitle();
            n["date"] = node->getDate().toString(Qt::ISODate);
            n["description"] = node->getDescription();

            QJsonArray att;
            for (auto& a : node->getAttachments())
                att.append(a);
            n["attachments"] = att;

            nodesArray.append(n);
        }
    }

    // ---- Save edges ----
    for (QGraphicsItem* item : timeline->getScene()->items()) {
        if (auto* edge = dynamic_cast<ConnectionItem*>(item)) {

            NodeItem* start = edge->getStartNode();
            NodeItem* end = edge->getEndNode();

            if (!start || !end)
                continue;

            if (!idMap.contains(start) || !idMap.contains(end))
                continue;

            QJsonObject e;
            e["from"] = idMap.value(start);
            e["to"] = idMap.value(end);

            edgesArray.append(e);
        }
    }


    root["nodes"] = nodesArray;
    root["edges"] = edgesArray;

    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
        file.close();
    }
}
void MainWindow::loadProject()
{
    QString path = QFileDialog::getOpenFileName(
        this, "Open Project", "", "Line Files (*.line)");

    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject root = doc.object();

    timeline->clearAll();
    QMap<int, NodeItem*> nodeMap;

    // ---- Load nodes ----
    for (auto v : root["nodes"].toArray()) {
        QJsonObject n = v.toObject();

        NodeItem* node = new NodeItem();
        node->setId(n["id"].toInt());
        node->setPos(n["x"].toDouble(), n["y"].toDouble());
        node->setTitle(n["title"].toString());
        node->setDate(QDate::fromString(n["date"].toString(), Qt::ISODate));
        node->setDescription(n["description"].toString());

        for (auto a : n["attachments"].toArray())
            node->addAttachment(a.toString());

        timeline->getScene()->addItem(node);
        nodeMap[node->getId()] = node;
    }

    // ---- Load edges ----
    for (auto v : root["edges"].toArray()) {
        QJsonObject e = v.toObject();

        NodeItem* a = nodeMap[e["from"].toInt()];
        NodeItem* b = nodeMap[e["to"].toInt()];

        if (a && b)
            timeline->getScene()->addItem(new ConnectionItem(a, b));
    }
}
void MainWindow::newProject()
{
    timeline->clearAll();
    currentNode = nullptr;

    titleEdit->clear();
    descriptionEdit->clear();
    attachmentsList->clear();
    dateEdit->setDate(QDate::currentDate());
}


