#include "ElementItem.hpp"

ElementItem::ElementItem(QObject *parent)
    : AbstractItem{parent}
{

}

void ElementItem::SetOrderId(int orderId)
{
    m_orderId = orderId;
}

int ElementItem::GetOrderId() const
{
    return m_orderId;
}

void ElementItem::SetValue(bool value)
{
    m_outputValue = value;
}

bool ElementItem::GetValue() const
{
    return m_outputValue;
}

void ElementItem::SetLogicalVector(const std::vector<bool>& lv)
{
    m_logicalVector = lv;
}

const std::vector<bool>& ElementItem::GetLogicalVector() const
{
    return m_logicalVector;
}

void ElementItem::SetInputsSize(std::size_t inputsSize)
{
    m_inputsSize = inputsSize;
}

std::size_t ElementItem::GetInputsSize() const
{
    return m_inputsSize;
}
