#include "CircuitOutput.hpp"
#include "connectors/EndingConnector.hpp"
#include "connectors/StartingConnector.hpp"
#include "Config.hpp"

#include <QPainter>

CircuitOutput::CircuitOutput(const CircuitOutputMimeData& mimeData,
                             QWidget *parent)
    : BaseCircuitItem{parent}
{
    m_color = Qt::darkGray;

    setFixedSize(80, 30);
    m_pixmap = QPixmap(this->size());
    m_pixmap.fill(QColor(Qt::transparent));

    m_endingConnectors.resize(1);

    QPoint positionOffset(3, 9);
    auto* endingConnector = new EndingConnector(mimeData.endingPoints.at(0),
                                                positionOffset,
                                                this);
    endingConnector->move(positionOffset);
    m_endingConnectors.at(0) = endingConnector;

    m_id = mimeData.id;
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
                                         m_endingConnectors.at(0)->GetPositionOffset());
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
    mimeData.itemSize = this->size();
    mimeData.value = m_outputValue;
    mimeData.itemPosition = pos();
    mimeData.area = QRect(mimeData.itemPosition, mimeData.itemSize);
    mimeData.color = m_color;

    EndingPointVector oldEndingPointVector;
    StartingPointVector oldStartingPointVector;
    for (const auto& endingConnector : m_endingConnectors)
    {
        const auto endPoint = endingConnector->GetEndPoint();
        oldEndingPointVector.push_back(endPoint);
        mimeData.endingPoints.push_back({endPoint.connPos,
                                         endPoint.connId});
    }

    for (const auto* startingConnector : m_startingConnectors)
    {
        const auto startPoint = startingConnector->GetStartPoint();
        oldStartingPointVector.push_back(startPoint);
        mimeData.startingPoints.push_back({startPoint.connPos,
                                           startPoint.connIds});
    }

    for (unsigned int i = 0; i < mimeData.endingPoints.size(); ++i)
    {
        mimeData.oldNewPoints.push_back({oldEndingPointVector[i].connPos,
                                         mimeData.endingPoints[i].connPos});
    }

    for (unsigned int i = 0; i < mimeData.startingPoints.size(); ++i)
    {
        mimeData.oldNewPoints.push_back({oldStartingPointVector[i].connPos,
                                         mimeData.startingPoints[i].connPos});
    }

    return mimeData;
}

void CircuitOutput::RemoveConnectionId(quint64 connId)
{
    m_endingConnectors.at(0)->RemoveConnectionId(connId);
}
