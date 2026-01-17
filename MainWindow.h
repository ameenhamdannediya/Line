#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QTextEdit>
#include <QDateEdit>
#include <QListWidget>

class TimelineView;
class NodeItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onNodeSelected(NodeItem* node);   

private:
    TimelineView* timeline = nullptr;
    QLineEdit* titleEdit = nullptr;
    QTextEdit* descriptionEdit = nullptr;
    QListWidget* attachmentsList = nullptr;
    QDateEdit* dateEdit = nullptr;
    NodeItem* currentNode = nullptr;       

    // save and load ile here 
protected:
    void keyPressEvent(QKeyEvent* event) override;
private:
    void newProject();
    void saveProject();
    void loadProject();


};
