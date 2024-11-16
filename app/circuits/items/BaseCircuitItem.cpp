#include "BaseCircuitItem.hpp"

BaseCircuitItem::BaseCircuitItem(QWidget *parent)
    : QWidget{parent}
{

}

void BaseCircuitItem::SetId(quint64 id)
{
    m_id = id;
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
