#include "BaseCircuitItem.hpp"
#include "connectors/EndingConnector.hpp"
#include "connectors/StartingConnector.hpp"

#include <QPainter>

BaseCircuitItem::BaseCircuitItem(QWidget *parent)
    : QWidget{parent}
{

}

BaseCircuitItem::~BaseCircuitItem()
{
    m_endingConnectors.clear();
    m_startingConnectors.clear();
}

quint64 BaseCircuitItem::GetId() const
{
    return m_id;
}

std::vector<EndingConnector*> BaseCircuitItem::GetEndingConnectors() const
{
    return m_endingConnectors;
}

std::vector<StartingConnector*> BaseCircuitItem::GetStartingConnectors() const
{
    return m_startingConnectors;
}

void BaseCircuitItem::RemoveConnectionId(quint64 connId)
{
    for (auto* startingConnector : m_startingConnectors)
    {
        startingConnector->RemoveConnectionId(connId);
    }

    for (auto* endingConnector : m_endingConnectors)
    {
        endingConnector->RemoveConnectionId(connId);
    }
}

void BaseCircuitItem::SetOrderId(int orderId)
{
    m_orderId = orderId;
    update();
}

void BaseCircuitItem::SetColor(const QColor& color)
{
    m_color = color;
    update();
}

void BaseCircuitItem::paintEvent(QPaintEvent* event)
{
    DrawToPixmap();

    QPainter painter(this);
    painter.drawPixmap(0, 0, m_pixmap);
}
