#include "BaseConnector.hpp"
#include "circuits/items/BaseCircuitItem.hpp"

#include <QPainter>

BaseConnector::BaseConnector(QWidget* parent)
    : QWidget(parent)
{
    m_sourceItem = qobject_cast<BaseCircuitItem*>(parent);

    auto size = QSize(12, 12);
    setMinimumSize(size);
    setMaximumSize(size);
    m_pixmap = QPixmap(size);
    m_pixmap.fill(QColor(Qt::transparent));
}

quint64 BaseConnector::GetItemId() const
{
    return m_sourceItem ? m_sourceItem->GetId() : 0;
}

QPoint BaseConnector::GetPositionOffset() const
{
    return m_positionOffset;
}

QPixmap BaseConnector::GetPixmap() const
{
    return m_pixmap;
}

void BaseConnector::DrawToPixmap()
{
    QPainter painter(&m_pixmap);

    DrawConnectorToPixmap(painter, QPoint(0, 0));
}

void BaseConnector::DrawConnectorToPixmap(QPainter& painter, QPoint positionOffset)
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

void BaseConnector::paintEvent(QPaintEvent* /* event */)
{
    DrawToPixmap();

    QPainter painter(this);
    painter.drawPixmap(0, 0, m_pixmap);
}

