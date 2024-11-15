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

void BaseCircuitItem::SetSize(QSize size)
{
    m_size = size;
}

QSize BaseCircuitItem::GetSize() const
{
    return m_size;
}

std::vector<EndingConnector*> BaseCircuitItem::GetEndingConnectors() const
{
    return m_endingConnectors;
}

std::vector<StartingConnector*> BaseCircuitItem::GetStartingConnectors() const
{
    return m_startingConnectors;
}
