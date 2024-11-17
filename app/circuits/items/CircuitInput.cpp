#include "CircuitInput.hpp"
#include "connectors/EndingConnector.hpp"
#include "connectors/StartingConnector.hpp"
#include "Config.hpp"

#include <QPainter>

CircuitInput::CircuitInput(const CircuitInputMimeData& mimeData,
                           QWidget* parent)
    : BaseCircuitItem{parent}
{
    setFixedSize(80, 30);
    m_pixmap = QPixmap(this->size());
    m_pixmap.fill(QColor(Qt::transparent));

    m_startingConnectors.resize(1);

    QPoint positionOffset(65, 9);
    auto* startingConnector = new StartingConnector(mimeData.startingPoints.at(0),
                                                    positionOffset,
                                                    this);
    startingConnector->move(positionOffset);
    m_startingConnectors.at(0) = startingConnector;

    m_id = mimeData.id;
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
                            m_startingConnectors.at(0)->GetPositionOffset());
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
    mimeData.itemSize = this->size();
    mimeData.value = m_inputValue;
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

void CircuitInput::RemoveConnectionId(quint64 connId)
{
    m_startingConnectors.at(0)->RemoveConnectionId(connId);
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
