#include "MainWindow.hpp"
#include "circuits/CircuitCanvas.hpp"
#include "logic/LogicController.hpp"
#include "dialogues/DialogCreateInputItem.hpp"
#include "dialogues/DialogCreateOutputItem.hpp"
#include "dialogues/DialogCreateElementItem.hpp"
#include "presenters/SimulationPresenter.hpp"
#include "ui_MainWindow.h"

#include <QDebug>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setMinimumSize(400, 300);

    ui->setupUi(this);

    m_logicController = new LogicController(this);
    m_canvas = new CircuitCanvas(this);
    m_simulationPresenter = new SimulationPresenter(this);

    connect(m_canvas, &CircuitCanvas::addNewInputItem,
            m_logicController, &LogicController::AddNewInputItem);

    connect(m_canvas, &CircuitCanvas::addNewOutputItem,
            m_logicController, &LogicController::AddNewOutputItem);

    connect(m_canvas, &CircuitCanvas::addNewElementItem,
            m_logicController, &LogicController::AddNewElementItem);

    connect(m_canvas, &CircuitCanvas::removeItem,
            m_logicController, &LogicController::RemoveItem);

    connect(m_canvas, &CircuitCanvas::changeElementItemInputsSize,
            m_logicController, &LogicController::ChangeElementItemInputsSize);

    connect(this, &MainWindow::newCircuitTriggered,
            m_logicController, &LogicController::ClearAllItems);
    connect(m_logicController, &LogicController::clearAllItems,
            m_canvas, &CircuitCanvas::CreateNewCircuit);

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

    auto orderId = m_canvas->GetInputOrderIdHint();

    const auto pos = QCursor::pos();

    m_createInputDialog = new DialogCreateInputItem(pos,
                                                    orderId,
                                                    this);

    connect(m_canvas, &CircuitCanvas::setInputOrderIdHint,
            m_createInputDialog, &DialogCreateInputItem::SetInputOrderIdHint);
}

void MainWindow::on_actionNew_Output_triggered()
{
    qDebug() << "Main window action: New Output triggered";

    auto orderId = m_canvas->GetOutputOrderIdHint();

    const auto pos = QCursor::pos();

    m_createOutputDialog = new DialogCreateOutputItem(pos,
                                                      orderId,
                                                      this);

    connect(m_canvas, &CircuitCanvas::setOutputOrderIdHint,
            m_createOutputDialog, &DialogCreateOutputItem::SetOutputOrderIdHint);
}

void MainWindow::on_actionNew_Element_triggered()
{
    qDebug() << "Main window action: New Element triggered";

    auto orderId = m_canvas->GetElementOrderIdHint();

    const auto pos = QCursor::pos();

    m_createElementDialog = new DialogCreateElementItem(pos,
                                                        orderId,
                                                        this);

    connect(m_canvas, &CircuitCanvas::setElementOrderIdHint,
            m_createElementDialog, &DialogCreateElementItem::SetElementOrderIdHint);
}

void MainWindow::on_actionNew_Circuit_triggered()
{
    qDebug() << "Main window action: New Circuit triggered";

    emit newCircuitTriggered();
}
