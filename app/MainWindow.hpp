#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CircuitCanvas;
class LogicController;
class DialogCreateInputItem;
class DialogCreateOutputItem;
class DialogCreateElementItem;
class SimulationPresenter;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void newCircuitTriggered();

private slots:
    void on_actionNew_Input_triggered();
    void on_actionNew_Output_triggered();
    void on_actionNew_Element_triggered();
    void on_actionNew_Circuit_triggered();
    void on_actionOpen_Circuit_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionExit_triggered();

private:
    Ui::MainWindow* ui;
    CircuitCanvas* m_canvas;
    LogicController* m_logicController;
    DialogCreateInputItem* m_createInputDialog = nullptr;
    DialogCreateOutputItem* m_createOutputDialog = nullptr;
    DialogCreateElementItem* m_createElementDialog = nullptr;
    SimulationPresenter* m_simulationPresenter;
};
#endif // MAINWINDOW_H
