#include "CircuitOutput.hpp"
#include "connectors/EndingConnector.hpp"
#include "Config.hpp"

#include <QPainter>

CircuitOutput::CircuitOutput(const CircuitOutputMimeData& mimeData,
                             QWidget *parent)
    : BaseCircuitItem{parent}
{
    setMinimumSize(80, 30);
    setMaximumSize(80, 30);
    SetSize(QSize(80, 30));
    m_pixmap = QPixmap(GetSize());
    m_pixmap.fill(QColor(Qt::transparent));

    m_endingConnectors.resize(1);

    QPoint positionOffset(3, 9);
    auto* endingConnector = new EndingConnector(mimeData.endPoint,
                                                positionOffset,
                                                this);
    endingConnector->move(positionOffset);
    m_endingConnectors[0] = endingConnector;

    SetId(mimeData.id);
    m_orderId = mimeData.orderId;
    m_outputValue = mimeData.value;
    if (mimeData.color.isValid())
    {
        m_color = mimeData.color;
    }

    CircuitOutput::DrawToPixmap();
    show();
    setAttribute(Qt::WA_DeleteOnClose);
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
                                         m_endingConnectors[0]->GetPositionOffset());
}

void CircuitOutput::SetValue(bool value)
{
    m_outputValue = value;
}

CircuitOutputMimeData CircuitOutput::GetMimeData(QPoint eventPos) const
{
    CircuitOutputMimeData mimeData(eventPos);
    mimeData.pixmap = m_pixmap;
    mimeData.offset = QPoint(eventPos - pos());
    mimeData.id = GetId();
    mimeData.orderId = m_orderId;
    mimeData.itemSize = GetSize();
    mimeData.value = m_outputValue;
    mimeData.itemPosition = pos();
    mimeData.area = QRect(mimeData.itemPosition, mimeData.itemSize);
    mimeData.color = m_color;

    const auto endPoint = m_endingConnectors[0]->GetEndPoint();
    mimeData.endPoint = endPoint;
    mimeData.oldEndPointPos = endPoint.connPos;
    mimeData.endOffset = QPoint(eventPos - endPoint.connPos);
    mimeData.connId = endPoint.connId;

    mimeData.oldNewPoints.emplace_back(mimeData.oldEndPointPos,
                                       mimeData.endPoint.connPos);

    return mimeData;
}

void CircuitOutput::RemoveConnectionId(quint64 connId)
{
    m_endingConnectors[0]->RemoveConnectionId(connId);
}

void CircuitOutput::SetOrderId(int orderId)
{
    m_orderId = orderId;
    update();
}

void CircuitOutput::SetColor(const QColor& color)
{
    m_color = color;
    update();
}

void CircuitOutput::paintEvent(QPaintEvent *event)
{
    DrawToPixmap();

    QPainter painter(this);
    painter.drawPixmap(0, 0, m_pixmap);
}
