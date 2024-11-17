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
{
    setAcceptDrops(true);
    m_areaManager.SetMatrixSize(QSize(1280, 720));

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
