#include "CircuitCanvas.hpp"
#include "items/CircuitInput.hpp"
#include "items/CircuitOutput.hpp"
#include "items/CircuitElement.hpp"

#include <QDebug>
#include <QPainter>

CircuitCanvas::CircuitCanvas(QWidget *parent)
    : QWidget{parent}
    , m_areaManager{this}
    , m_projectConfigurator{m_areaManager, m_idHandler, this}
    , m_itemRegistry{this}
{
    setAcceptDrops(true);
    m_areaManager.SetMatrixSize(QSize(1280, 720));

    connect(&m_itemRegistry, &ItemRegistry::removeCircuitItem,
            this, &CircuitCanvas::RemoveCircuitItem);

    connect(&m_itemRegistry, &ItemRegistry::setNumberParameterToElementItem,
            this, &CircuitCanvas::setNumberParameterToElementItem);

    connect(&m_itemRegistry, &ItemRegistry::startFunctionalFaultSimulation,
            this, &CircuitCanvas::startFunctionalFaultSimulation);

    connect(&m_itemRegistry, &ItemRegistry::askOrderIdHint,
            this, [this](){
                const auto orderId =
                    m_idHandler.GetLastOrderId(ItemType::Element);
                emit setOrderIdHint(ItemType::Element, orderId);
            });

    connect(this, &CircuitCanvas::setOrderIdHint,
            &m_itemRegistry, &ItemRegistry::setOrderIdHintForDuplicate);

    connect(&m_itemRegistry, &ItemRegistry::tryToRebookArea,
            this, &CircuitCanvas::TryToRebookArea);

    m_itemRegistry.RegisterMimeProcessor(inputMime, ItemType::Input,
                            CircuitInput::ConstructCircuitInputFromStream);
    m_itemRegistry.RegisterMimeProcessor(outputMime, ItemType::Output,
                            CircuitOutput::ConstructCircuitOutputFromStream);
    m_itemRegistry.RegisterMimeProcessor(elementMime, ItemType::Element,
                            CircuitElement::ConstructCircuitElementFromStream);

    BaseCircuitItem::RegisterJsonProcessor(ItemType::Input,
                            CircuitInput::ConstructCircuitInputFromJson);
    BaseCircuitItem::RegisterJsonProcessor(ItemType::Output,
                            CircuitOutput::ConstructCircuitOutputFromJson);
    BaseCircuitItem::RegisterJsonProcessor(ItemType::Element,
                            CircuitElement::ConstructCircuitElementFromJson);

    connect(&m_projectConfigurator, &ProjectConfigurationManager::addNewItem,
            this, &CircuitCanvas::addNewItem);
    connect(&m_projectConfigurator, &ProjectConfigurationManager::insertConnection,
            this, &CircuitCanvas::InsertConnection);
    connect(&m_projectConfigurator, &ProjectConfigurationManager::clearCircuit,
            this, &CircuitCanvas::ClearAll);
}

int CircuitCanvas::GetOrderIdHint(quint64 itemType)
{
    return m_idHandler.GetLastOrderId(itemType);
}

void CircuitCanvas::CreateNewCircuit()
{
    m_projectConfigurator.ResetCircuit();
}

void CircuitCanvas::OpenCircuitFromFile()
{
    m_projectConfigurator.OpenCircuitFromFile();
}

void CircuitCanvas::SaveCircuitToFile()
{
    m_projectConfigurator.SaveCircuitToFile();
}

void CircuitCanvas::NewSavingFile()
{
    m_projectConfigurator.NewSavingFile();
}

void CircuitCanvas::TryToRebookArea(CircuitElement* circuitElement,
                                    QRect previousArea,
                                    std::vector<std::pair<QPoint, QPoint> > oldNewPoints,
                                    quint64 displacedConnId,
                                    StartingPoint::IdsSet displacedConnIdSet,
                                    int previousInputsNumber,
                                    int previousOutputsNumber)
{
    m_areaManager.ClearAndBackupArea(previousArea);

    QRect area(circuitElement->pos(), circuitElement->size());
    if (m_areaManager.TryBookArea(area, oldNewPoints))
    {
        // Successfully booked new area
        // Decrementing case of inputs size
        if (displacedConnId)
        {
            RemoveConnectionById(displacedConnId);
        }

        // Decrementing case of outputs size
        if (!displacedConnIdSet.empty())
        {
            for (const auto& connId : displacedConnIdSet)
            {
                RemoveConnectionById(connId);
            }
        }

        emit changeElementItemInputsSize(circuitElement->GetId(),
                                         circuitElement->GetEndingConnectors().size());
    }
    else
    {
        // Can't book new area
        circuitElement->SetInputsNumber(previousInputsNumber);
        emit circuitElement->inputsNumber(previousInputsNumber);
        circuitElement->SetOutputsNumber(previousOutputsNumber);
        emit circuitElement->outputsNumber(previousOutputsNumber);
    }

    // To redraw connections
    update();
}

void CircuitCanvas::paintEvent(QPaintEvent *event)
{
    (void)event;

    QPainter painter(this);
    QPen mPen;

#ifdef VISUAL_DEBUGGING
    // For debug purposes
    mPen.setColor(Qt::lightGray);
    mPen.setWidth(1);
    painter.setPen(mPen);
    for (const auto& line : m_areaManager.GetNetLines())
        painter.drawLine(line);

    mPen.setColor(Qt::red);
    mPen.setWidth(1);
    painter.setPen(mPen);
    painter.setBrush(Qt::red);
    for (const auto& point : m_areaManager.GetBookedPoints())
    {
        if (point.second)
            painter.drawEllipse(point.first, 2, 2);
    }

    mPen.setColor(Qt::blue);
    mPen.setWidth(1);
    painter.setPen(mPen);
    painter.setBrush(Qt::blue);
    for (const auto& point : m_areaManager.GetBookedPoints())
    {
        if (!point.second)
            painter.drawEllipse(point.first, 2, 2);
    }
#endif // VISUAL_DEBUGGING

    mPen.setColor(Qt::black);
    mPen.setWidth(2);
    painter.setPen(mPen);
    painter.drawLine(m_currentConnectingLine);
    for (const auto& line : m_areaManager.GetConnectionPolygons())
    {
        //qDebug() << "Drawing line -> { start =" << line.front()
        //         << "end =" << line.back() << '}';
        painter.drawPolyline(line);
    }
}

void CircuitCanvas::dragEnterEvent(QDragEnterEvent *event)
{
    ProcessDragEnterEvent(event);
}

void CircuitCanvas::dragMoveEvent(QDragMoveEvent *event)
{
    ProcessDragMoveEvent(event);
}

void CircuitCanvas::dragLeaveEvent(QDragLeaveEvent*)
{
    m_currentConnectingLine = {};
    update();
}

void CircuitCanvas::dropEvent(QDropEvent *event)
{
    ProcessDropEvent(event);
}

void CircuitCanvas::mousePressEvent(QMouseEvent *event)
{
    ProcessMousePressEvent(event);
}
