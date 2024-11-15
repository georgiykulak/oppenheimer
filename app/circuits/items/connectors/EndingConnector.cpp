#include "EndingConnector.hpp"
#include "circuits/items/BaseCircuitItem.hpp"

#include <QDebug>
#include <QPainter>

EndingConnector::EndingConnector(const EndingPoint& endPoint,
                                 QPoint positionOffset,
                                 QWidget* parent)
    : QWidget{parent}
{
    auto size = QSize(12, 12);
    setMinimumSize(size);
    setMaximumSize(size);
    m_pixmap = QPixmap(size);
    m_pixmap.fill(QColor(Qt::transparent));

    m_endPoint = endPoint;
    m_positionOffset = positionOffset;

    m_sourceItem = qobject_cast<BaseCircuitItem*>(parent);
}

quint64 EndingConnector::GetItemId() const
{
    return m_sourceItem ? m_sourceItem->GetId() : 0;
}

const EndingPoint& EndingConnector::GetEndPoint() const
{
    return m_endPoint;
}

QPoint EndingConnector::GetPositionOffset() const
{
    return m_positionOffset;
}

std::size_t EndingConnector::GetConnectionId() const
{
    return m_endPoint.connId;
}

bool EndingConnector::IsConnected() const
{
    return static_cast<bool>(m_endPoint.connId);
}

void EndingConnector::SetPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
}

QPixmap EndingConnector::GetPixmap() const
{
    return m_pixmap;
}

void EndingConnector::DrawToPixmap()
{
    QPainter painter(&m_pixmap);

    DrawConnectorToPixmap(painter, QPoint(0, 0));
}

void EndingConnector::DrawConnectorToPixmap(QPainter& painter, QPoint positionOffset)
{
    Qt::GlobalColor color = Qt::black;
    QPen mPen;
    mPen.setColor(color);
    mPen.setWidth(2);
    painter.setPen(mPen);
    painter.setBrush(color);
    painter.drawEllipse(1 + positionOffset.x(),
                        1 + positionOffset.y(), 10, 10);
}

void EndingConnector::SetConnectionId(quint64 connId)
{
    qDebug() << "EndingConnector::InsertConnectionId called, connId =" << connId;

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

void EndingConnector::paintEvent(QPaintEvent *event)
{
    DrawToPixmap();

    QPainter painter(this);
    painter.drawPixmap(0, 0, m_pixmap);
}
