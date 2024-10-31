#include "CircuitInput.hpp"
#include "connectors/StartingConnector.hpp"

#include <QPainter>

CircuitInput::CircuitInput(const StartingPoint& startPoint, QWidget* parent)
    : BaseCircuitItem{parent}
{
    setMinimumSize(80, 30);
    setMaximumSize(80, 30);
    SetSize(QSize(80, 30));
    m_pixmap = QPixmap(GetSize());
    m_pixmap.fill(QColor(Qt::transparent));

    QPoint positionOffset(65, 9);
    m_startingConnector = new StartingConnector(startPoint, positionOffset, this);

    m_startingConnector->move(positionOffset);
    m_startingConnector->update();
    m_startingConnector->show();
    m_startingConnector->setAttribute(Qt::WA_DeleteOnClose);
}

void CircuitInput::SetPixmap(const QPixmap& pixmap)
{
    m_pixmap = pixmap;
}

QPixmap CircuitInput::GetPixmap() const
{
    return m_pixmap;
}

void CircuitInput::DrawToPixmap()
{
    QPainter painter(&m_pixmap);

    int arr[10] = {20,0, 50,0, 70,15, 50,30, 20,30};
    QPen mPen;
    mPen.setColor(m_color);
    mPen.setWidth(2);
    painter.setPen(mPen);
    painter.setBrush(m_color);
    QPolygon poly;
    poly.setPoints(5, arr);
    painter.drawPolygon(poly);

    mPen.setColor(Qt::gray);
    painter.setPen(mPen);
    painter.setFont(QFont("Arial"));
    QString strVal;
    strVal.setNum(m_inputValue);
    painter.drawText(5, 20, strVal);

    mPen.setColor(Qt::black);
    painter.setPen(mPen);
    painter.setFont(QFont("Arial"));
    QString strId;
    strId.setNum(m_orderId);
    painter.drawText(25, 20, strId);

    StartingConnector::DrawConnectorToPixmap(painter,
                            m_startingConnector->GetPositionOffset());
}

void CircuitInput::SetOrderId(int orderId)
{
    m_orderId = orderId;
}

int CircuitInput::GetOrderId() const
{
    return m_orderId;
}

void CircuitInput::SetValue(bool value)
{
    m_inputValue = value;
}

bool CircuitInput::GetValue() const
{
    return m_inputValue;
}

void CircuitInput::SetColor(const QColor& color)
{
    m_color = color;
}

QColor CircuitInput::GetColor() const
{
    return m_color;
}

const StartingPoint& CircuitInput::GetStartPoint() const
{
    return m_startingConnector->GetStartPoint();
}

void CircuitInput::RemoveConnectionId(quint64 connId)
{
    m_startingConnector->RemoveConnectionId(connId);
}

void CircuitInput::paintEvent(QPaintEvent *event)
{
    DrawToPixmap();

    QPainter painter(this);
    painter.drawPixmap(0, 0, m_pixmap);
}
