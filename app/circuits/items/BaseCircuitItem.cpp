#include "BaseCircuitItem.hpp"

BaseCircuitItem::BaseCircuitItem(QWidget *parent)
    : QWidget{parent}
{

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
