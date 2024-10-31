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

bool IdHandler::NewInputOrderId(int orderId)
{
    auto iter = m_inputOrderIds.find(orderId);
    if (iter != m_inputOrderIds.end())
        return false;

    m_inputOrderIds.insert(orderId);

    if (orderId > m_lastInputOrderId)
        m_lastInputOrderId = orderId;

    ++m_lastInputOrderId;

    return true;
}

bool IdHandler::NewOutputOrderId(int orderId)
{
    auto iter = m_outputOrderIds.find(orderId);
    if (iter != m_outputOrderIds.end())
        return false;

    m_outputOrderIds.insert(orderId);

    if (orderId > m_lastOutputOrderId)
        m_lastOutputOrderId = orderId;

    ++m_lastOutputOrderId;

    return true;
}

bool IdHandler::NewElementOrderId(int orderId)
{
    auto iter = m_elementOrderIds.find(orderId);
    if (iter != m_elementOrderIds.end())
        return false;

    m_elementOrderIds.insert(orderId);

    if (orderId > m_lastElementOrderId)
        m_lastElementOrderId = orderId;

    ++m_lastElementOrderId;

    return true;
}

quint64 IdHandler::ContainsUid(quint64 uid) const
{
    return m_uids.contains(uid);
}

bool IdHandler::ContainsInputOrderId(int orderId) const
{
    return m_inputOrderIds.contains(orderId);
}

bool IdHandler::ContainsOutputOrderId(int orderId) const
{
    return m_outputOrderIds.contains(orderId);
}

bool IdHandler::ContainsElementOrderId(int orderId) const
{
    return m_elementOrderIds.contains(orderId);
}

int IdHandler::GetLastInputOrderId() const
{
    return m_lastInputOrderId;
}

int IdHandler::GetLastOutputOrderId() const
{
    return m_lastOutputOrderId;
}

int IdHandler::GetLastElementOrderId() const
{
    return m_lastElementOrderId;
}

void IdHandler::Clear()
{
    m_lastUid = 1;
    m_lastInputOrderId = 0;
    m_lastOutputOrderId = 0;
    m_lastElementOrderId = 0;
}
