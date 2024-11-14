#include "StartingConnector.hpp"
#include "circuits/items/BaseCircuitItem.hpp"

#include <QDebug>
#include <QPainter>

StartingConnector::StartingConnector(const StartingPoint& startPoint,
                                     QPoint positionOffset,
                                     QWidget* parent)
    : QWidget{parent}
{
    auto size = QSize(12, 12);
    setMinimumSize(size);
    setMaximumSize(size);
    m_pixmap = QPixmap(size);
    m_pixmap.fill(QColor(Qt::transparent));

    m_startPoint = startPoint;
    m_positionOffset = positionOffset;

    m_sourceItem = qobject_cast<BaseCircuitItem*>(parent);
}

quint64 StartingConnector::GetItemId() const
{
    return m_sourceItem ? m_sourceItem->GetId() : 0;
}

const StartingPoint& StartingConnector::GetStartPoint() const
{
    return m_startPoint;
}

QPoint StartingConnector::GetPositionOffset() const
{
    return m_positionOffset;
}

void StartingConnector::SetPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
}

QPixmap StartingConnector::GetPixmap() const
{
    return m_pixmap;
}

void StartingConnector::DrawToPixmap()
{
    QPainter painter(&m_pixmap);

    DrawConnectorToPixmap(painter, QPoint(0, 0));
}

void StartingConnector::DrawConnectorToPixmap(QPainter& painter, QPoint positionOffset)
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

void StartingConnector::InsertConnectionId(quint64 connId)
{
    qDebug() << "StartingConnector::InsertConnectionId called, connections ="
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

void StartingConnector::paintEvent(QPaintEvent *event)
{
    DrawToPixmap();

    QPainter painter(this);
    painter.drawPixmap(0, 0, m_pixmap);
}
