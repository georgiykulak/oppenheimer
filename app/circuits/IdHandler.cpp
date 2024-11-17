#include "IdHandler.hpp"

IdHandler::IdHandler(QObject *parent)
    : QObject{parent}
{

}

quint64 IdHandler::NewUid()
{
    const auto uid = m_lastUid;
    ++m_lastUid;
    m_uids.insert(uid);

    return uid;
}

bool IdHandler::InsertUid(quint64 id)
{
    if (m_uids.contains(id))
    {
        return false;
    }

    m_uids.insert(id);

    m_lastUid = *std::max_element(m_uids.begin(), m_uids.end());
    ++m_lastUid;

    return true;
}

void IdHandler::RemoveUid(quint64 id)
{
    if (!m_uids.contains(id))
    {
        return;
    }

    m_uids.erase(id);
}

bool IdHandler::NewOrderId(quint64 itemType, int orderId)
{
    auto& orderIds = m_itemOrderIds[itemType];

    if (orderIds.contains(orderId))
    {
        return false;
    }

    orderIds.insert(orderId);

    auto& lastOrderId = m_lastItemOrderId[itemType];
    lastOrderId = *std::max_element(orderIds.begin(), orderIds.end());
    ++lastOrderId;

    return true;
}

void IdHandler::RemoveOrderId(quint64 itemType, int orderId)
{
    if (!m_itemOrderIds.contains(itemType))
    {
        return;
    }

    auto& orderIds = m_itemOrderIds[itemType];

    if (!orderIds.contains(orderId))
    {
        return;
    }

    orderIds.erase(orderId);
}

bool IdHandler::ContainsOrderId(quint64 itemType, int orderId) const
{
    if (!m_itemOrderIds.contains(itemType))
    {
        return false;
    }

    const auto& orderIds = m_itemOrderIds.at(itemType);

    return orderIds.contains(orderId);
}

int IdHandler::GetLastOrderId(quint64 itemType) const
{
    if (!m_lastItemOrderId.contains(itemType))
    {
        return 0;
    }

    return m_lastItemOrderId.at(itemType);
}

void IdHandler::Clear()
{
    m_itemOrderIds.clear();
    m_lastItemOrderId.clear();
    m_uids.clear();
    m_lastUid = 1;
}
