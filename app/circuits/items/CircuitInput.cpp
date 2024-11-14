#include "CircuitInput.hpp"
#include "connectors/StartingConnector.hpp"
#include "Config.hpp"

#include <QPainter>

CircuitInput::CircuitInput(const CircuitInputMimeData& mimeData,
                           QWidget* parent)
    : BaseCircuitItem{parent}
{
    setMinimumSize(80, 30);
    setMaximumSize(80, 30);
    SetSize(QSize(80, 30));
    m_pixmap = QPixmap(GetSize());
    m_pixmap.fill(QColor(Qt::transparent));

    QPoint positionOffset(65, 9);
    m_startingConnector = new StartingConnector(mimeData.startPoint,
                                                positionOffset,
                                                this);
    m_startingConnector->move(positionOffset);
    m_startingConnector->update();
    m_startingConnector->show();
    m_startingConnector->setAttribute(Qt::WA_DeleteOnClose);

    SetId(mimeData.id);
    m_orderId = mimeData.orderId;
    m_inputValue = mimeData.value;
    if (mimeData.color.isValid())
    {
        m_color = mimeData.color;
    }

    CircuitInput::DrawToPixmap();
    show();
    setAttribute(Qt::WA_DeleteOnClose);
}

void CircuitInput::DrawToPixmap()
{
    QPainter painter(&m_pixmap);

    int arr[10] = {20,0, 50,0, 70,15, 50,30, 20,30};
    QPen mPen;
    mPen.setColor(m_color);
    painter.setPen(mPen);
    painter.setBrush(m_color);
    QPolygon poly;
    poly.setPoints(5, arr);
    painter.drawPolygon(poly);

#ifdef DRAW_INPUT_ITEM_BORDERS
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

void CircuitInput::SetValue(bool value)
{
    m_inputValue = value;
}

CircuitInputMimeData CircuitInput::GetMimeData(QPoint eventPos) const
{
    CircuitInputMimeData mimeData(eventPos);
    mimeData.pixmap = m_pixmap;
    mimeData.offset = QPoint(eventPos - pos());
    mimeData.id = GetId();
    mimeData.orderId = m_orderId;
    mimeData.itemSize = GetSize();
    mimeData.value = m_inputValue;
    mimeData.itemPosition = pos();
    mimeData.area = QRect(mimeData.itemPosition, mimeData.itemSize);
    const auto startPoint = m_startingConnector->GetStartPoint();
    mimeData.oldStartPointPos = startPoint.connPos;
    mimeData.startOffset = QPoint(eventPos - startPoint.connPos);
    mimeData.connIdsNumber = (unsigned int)(startPoint.connIds.size());
    mimeData.color = m_color;
    mimeData.startPoint = startPoint;
    mimeData.oldNewPoints.emplace_back(mimeData.oldStartPointPos,
                                       mimeData.startPoint.connPos);

    return mimeData;
}

void CircuitInput::RemoveConnectionId(quint64 connId)
{
    m_startingConnector->RemoveConnectionId(connId);
}

void CircuitInput::SetOrderId(int orderId)
{
    m_orderId = orderId;
    update();
}

void CircuitInput::SetColor(const QColor& color)
{
    m_color = color;
    update();
}

void CircuitInput::paintEvent(QPaintEvent *event)
{
    DrawToPixmap();

    QPainter painter(this);
    painter.drawPixmap(0, 0, m_pixmap);
}
