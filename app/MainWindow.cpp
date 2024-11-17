#include "MainWindow.hpp"
#include "circuits/CircuitCanvas.hpp"
#include "logic/LogicController.hpp"
#include "dialogues/DialogCreateInputItem.hpp"
#include "dialogues/DialogCreateOutputItem.hpp"
#include "dialogues/DialogCreateElementItem.hpp"
#include "presenters/SimulationPresenter.hpp"
#include "ItemUtils.hpp"
#include "ui_MainWindow.h"

#include <QDebug>
#include <QKeyEvent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setMinimumSize(400, 300);

    ui->setupUi(this);

    m_logicController = new LogicController(this);
    m_canvas = new CircuitCanvas(this);
    m_simulationPresenter = new SimulationPresenter(this);

    connect(m_canvas, &CircuitCanvas::addNewItem,
            m_logicController, &LogicController::AddNewItem);

    connect(m_canvas, &CircuitCanvas::removeItem,
            m_logicController, &LogicController::RemoveItem);

    connect(m_canvas, &CircuitCanvas::changeElementItemInputsSize,
            m_logicController, &LogicController::ChangeElementItemInputsSize);

    connect(this, &MainWindow::newCircuitTriggered,
            m_canvas, &CircuitCanvas::CreateNewCircuit);
    connect(m_canvas, &CircuitCanvas::clearAllItems,
            m_logicController, &LogicController::ClearAllItems);

    connect(this, &MainWindow::openCircuitTriggered,
            m_canvas, &CircuitCanvas::OpenCircuitFromFile);

    connect(this, &MainWindow::saveTriggered,
            m_canvas, &CircuitCanvas::SaveCircuitToFile);

    connect(this, &MainWindow::saveAsTriggered,
            m_canvas, &CircuitCanvas::NewSavingFile);

    connect(m_canvas, &CircuitCanvas::setNumberParameterToElementItem,
            m_logicController, &LogicController::SetNumberParameterToElementItem);

    connect(m_canvas, &CircuitCanvas::startFunctionalFaultSimulation,
            m_logicController, &LogicController::StartFunctionalFaultSimulation);
    connect(m_logicController, &LogicController::presentDHQTable,
            m_simulationPresenter, &SimulationPresenter::PresentDHQTable);

    this->setCentralWidget(m_canvas);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionNew_Input_triggered()
{
    qDebug() << "Main window action: New Input triggered";

    auto orderId = m_canvas->GetOrderIdHint(ItemType::Input);

    const auto pos = QCursor::pos();

    m_createInputDialog = new DialogCreateInputItem(pos,
                                                    orderId,
                                                    this);

    connect(m_canvas, &CircuitCanvas::setOrderIdHint,
            m_createInputDialog, &DialogCreateInputItem::SetOrderIdHint);
}

void MainWindow::on_actionNew_Output_triggered()
{
    qDebug() << "Main window action: New Output triggered";

    auto orderId = m_canvas->GetOrderIdHint(ItemType::Output);

    const auto pos = QCursor::pos();

    m_createOutputDialog = new DialogCreateOutputItem(pos,
                                                      orderId,
                                                      this);

    connect(m_canvas, &CircuitCanvas::setOrderIdHint,
            m_createOutputDialog, &DialogCreateOutputItem::SetOrderIdHint);
}

void MainWindow::on_actionNew_Element_triggered()
{
    qDebug() << "Main window action: New Element triggered";

    auto orderId = m_canvas->GetOrderIdHint(ItemType::Element);

    const auto pos = QCursor::pos();

    m_createElementDialog = new DialogCreateElementItem(pos,
                                                        orderId,
                                                        this);

    connect(m_canvas, &CircuitCanvas::setOrderIdHint,
            m_createElementDialog, &DialogCreateElementItem::SetOrderIdHint);
}

void MainWindow::on_actionNew_Circuit_triggered()
{
    qDebug() << "Main window action: New Circuit triggered";

    emit newCircuitTriggered();
}

void MainWindow::on_actionOpen_Circuit_triggered()
{
    qDebug() << "Main window action: Open Circuit triggered";

    emit openCircuitTriggered();
}

void MainWindow::on_actionSave_triggered()
{
    qDebug() << "Main window action: Save triggered";

    emit saveTriggered();
}

void MainWindow::on_actionSave_As_triggered()
{
    qDebug() << "Main window action: Save As triggered";

    emit saveAsTriggered();
}

void MainWindow::on_actionExit_triggered()
{
    qDebug() << "Main window action: Exit triggered";

    auto button = QMessageBox::question(this, tr("Quit"),
                          tr("Are you sure you want to quit?"),
                          QMessageBox::Yes,
                          QMessageBox::No);

    if (button == QMessageBox::Yes)
    {
        close();
    }
}
