#include "CircuitOutput.hpp"
#include "connectors/EndingConnector.hpp"
#include "Config.hpp"

#include <QPainter>

CircuitOutput::CircuitOutput(const EndingPoint& endPoint, QWidget *parent)
    : BaseCircuitItem{parent}
{
    setMinimumSize(80, 30);
    setMaximumSize(80, 30);
    SetSize(QSize(80, 30));
    m_pixmap = QPixmap(GetSize());
    m_pixmap.fill(QColor(Qt::transparent));

    QPoint positionOffset(3, 9);
    m_endingConnector = new EndingConnector(endPoint, positionOffset, this);

    m_endingConnector->move(positionOffset);
    m_endingConnector->update();
    m_endingConnector->show();
    m_endingConnector->setAttribute(Qt::WA_DeleteOnClose);
}

void CircuitOutput::SetPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
}

QPixmap CircuitOutput::GetPixmap() const
{
    return m_pixmap;
}

void CircuitOutput::DrawToPixmap()
{
    QPainter painter(&m_pixmap);

    int arr[10] = {10,15, 30,0, 60,0, 60,30, 30,30};
    QPen mPen;
    mPen.setColor(m_color);
    painter.setPen(mPen);
    painter.setBrush(m_color);
    QPolygon poly;
    poly.setPoints(5, arr);
    painter.drawPolygon(poly);

#ifdef DRAW_OUTPUT_ITEM_BORDERS
    const int borderWidth = 2;
    mPen.setWidth(borderWidth);
    mPen.setColor(Qt::black);
    painter.setPen(mPen);
    painter.drawPolygon(poly);
#endif

    mPen.setWidth(2);
    mPen.setColor(Qt::gray);
    painter.setPen(mPen);
    painter.setFont(QFont("Arial"));
    QString strVal;
    strVal.setNum(m_outputValue);
    painter.drawText(65, 20, strVal);

    mPen.setColor(Qt::black);
    painter.setPen(mPen);
    painter.setFont(QFont("Arial"));
    QString strId;
    strId.setNum(m_orderId);
    painter.drawText(30, 20, strId);

    EndingConnector::DrawConnectorToPixmap(painter,
                                         m_endingConnector->GetPositionOffset());
}

void CircuitOutput::SetOrderId(int orderId)
{
    m_orderId = orderId;
}

int CircuitOutput::GetOrderId() const
{
    return m_orderId;
}

void CircuitOutput::SetValue(bool value)
{
    m_outputValue = value;
}

bool CircuitOutput::GetValue() const
{
    return m_outputValue;
}

void CircuitOutput::SetColor(const QColor& color)
{
    m_color = color;
}

QColor CircuitOutput::GetColor() const
{
    return m_color;
}

const EndingPoint& CircuitOutput::GetEndPoint() const
{
    return m_endingConnector->GetEndPoint();
}

void CircuitOutput::RemoveConnectionId(quint64 connId)
{
    m_endingConnector->RemoveConnectionId(connId);
}

void CircuitOutput::paintEvent(QPaintEvent *event)
{
    DrawToPixmap();

    QPainter painter(this);
    painter.drawPixmap(0, 0, m_pixmap);
}
