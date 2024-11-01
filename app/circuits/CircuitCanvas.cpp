#include "CircuitCanvas.hpp"
#include "items/BaseCircuitItem.hpp"

#include <QDebug>
#include <QMessageBox>
#include <QPainter>

CircuitCanvas::CircuitCanvas(QWidget *parent)
    : QWidget{parent}
    , m_areaManager{this}
{
    setAcceptDrops(true);
    m_areaManager.SetMatrixSize(QSize(1280, 720));
}

int CircuitCanvas::GetInputOrderIdHint()
{
    return m_idHandler.GetLastInputOrderId();
}

int CircuitCanvas::GetOutputOrderIdHint()
{
    return m_idHandler.GetLastOutputOrderId();
}

int CircuitCanvas::GetElementOrderIdHint()
{
    return m_idHandler.GetLastElementOrderId();
}

void CircuitCanvas::CreateNewCircuit()
{
    QObjectList childList = this->children();
    if (childList.size() > 1)
    {
        qWarning() << "You need to save current file before creating the new one";
    }

    for (auto* obj : childList)
    {
        auto* item = qobject_cast<BaseCircuitItem*>(obj);
        if (item)
        {
            RemoveCircuitItem(item);
        }
    }

    m_idHandler.Clear();

    m_areaManager.RemoveAllConnections();
    m_areaManager.ClearMatrix();

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
    for (const auto& line : m_areaManager.GetConnections())
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
