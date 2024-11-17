#include "BaseCircuitItem.hpp"

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

void BaseCircuitItem::SetOrderId(int orderId)
{
    m_orderId = orderId;
    update();
}

void BaseCircuitItem::paintEvent(QPaintEvent* event)
{
    DrawToPixmap();

    QPainter painter(this);
    painter.drawPixmap(0, 0, m_pixmap);
}
