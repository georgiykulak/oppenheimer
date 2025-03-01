#include "EndingConnector.hpp"
#include "circuits/items/BaseCircuitItem.hpp"

#include <QDebug>
#include <QMenu>

EndingConnector::EndingConnector(const EndingPoint& endPoint,
                                 QPoint positionOffset,
                                 QWidget* parent)
    : BaseConnector{parent}
{
    m_endPoint = endPoint;
    m_positionOffset = positionOffset;

    show();
    setAttribute(Qt::WA_DeleteOnClose);
}

QString EndingConnector::GetMimeType() const
{
    return "application/x-oph-endingconnector";
}

QPoint EndingConnector::GetConnectionPoint() const
{
    return m_endPoint.connPos;
}

const EndingPoint& EndingConnector::GetEndPoint() const
{
    return m_endPoint;
}

std::size_t EndingConnector::GetConnectionId() const
{
    return m_endPoint.connId;
}

bool EndingConnector::IsConnected() const
{
    return static_cast<bool>(m_endPoint.connId);
}

void EndingConnector::AddActionsToMenu(QMenu *menu)
{
    QAction* actionDisconnect = new QAction("Disconnect", this);
    connect(actionDisconnect, &QAction::triggered,
            this, [this] (bool) {
                emit removeConnectionById(m_endPoint.connId);
            });

    menu->addAction(actionDisconnect);
}

void EndingConnector::SetConnectionId(quint64 connId)
{
    qDebug() << "EndingConnector::SetConnectionId called, connId =" << connId;

    if (!m_endPoint.connId)
    {
        m_endPoint.connId = connId;
        qDebug() << "EndingConnector: Setted new connection ID:" << connId;
    }
}

void EndingConnector::RemoveConnectionId(quint64 connId)
{
    if (m_endPoint.connId != connId)
    {
        return;
    }

    m_endPoint.connId = 0;
    qDebug() << "EndingConnector: Disconnected, connection ID was:" << connId;
}
