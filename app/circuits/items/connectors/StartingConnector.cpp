#include "StartingConnector.hpp"
#include "circuits/items/BaseCircuitItem.hpp"

#include <QDebug>
#include <QMenu>

StartingConnector::StartingConnector(const StartingPoint& startPoint,
                                     QPoint positionOffset,
                                     QWidget* parent)
    : BaseConnector{parent}
{
    m_startPoint = startPoint;
    m_positionOffset = positionOffset;

    show();
    setAttribute(Qt::WA_DeleteOnClose);
}

QString StartingConnector::GetMimeType() const
{
    return "application/x-oph-startingconnector";
}

QPoint StartingConnector::GetConnectionPoint() const
{
    return m_startPoint.connPos;
}

const StartingPoint& StartingConnector::GetStartPoint() const
{
    return m_startPoint;
}

void StartingConnector::AddActionsToMenu(QMenu *menu)
{
    const auto connIdsSize = m_startPoint.connIds.size();
    QAction* actionDisconnect =
        new QAction(connIdsSize > 1 ? "Disconnect All" : "Disconnect", this);
    connect(actionDisconnect, &QAction::triggered,
            this, [this] (bool) {
                const auto connIdSet = m_startPoint.connIds;
                for (const auto& connId : connIdSet)
                {
                    emit removeConnectionById(connId);
                }
            });

    menu->addAction(actionDisconnect);
}

void StartingConnector::SetConnectionId(quint64 connId)
{
    qDebug() << "StartingConnector::SetConnectionId called, connections ="
             << m_startPoint.connIds.size() << "connId =" << connId;
    if (m_startPoint.connIds.contains(connId))
    {
        return;
    }

    m_startPoint.connIds.insert(connId);
    qDebug() << "StartingConnector: Inserted new connection ID:" << connId
             << "connections =" << m_startPoint.connIds.size();
}

void StartingConnector::RemoveConnectionId(quint64 connId)
{
    if (!m_startPoint.connIds.contains(connId))
    {
        return;
    }

    m_startPoint.connIds.erase(connId);
    qDebug() << "StartingConnector: Removed connection ID:" << connId
             << "connections =" << m_startPoint.connIds.size();
}
