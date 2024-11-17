#include "BaseCircuitItem.hpp"
#include "connectors/EndingConnector.hpp"
#include "connectors/StartingConnector.hpp"

#include <QPainter>

BaseCircuitItem::BaseCircuitItem(QWidget *parent)
    : QWidget{parent}
{

}

BaseCircuitItem::~BaseCircuitItem()
{
    m_endingConnectors.clear();
    m_startingConnectors.clear();
}

void BaseCircuitItem::RegisterJsonProcessor(quint64 type,
                                            JsonProcessor processor)
{
    if (type == ItemType::Invalid)
    {
        qCritical()
            << "RegisterJsonProcessor: Can't register processor with invalid type";
        return;
    }

    if (GetJsonProcessors().contains(type))
    {
        qWarning() << "RegisterJsonConstructors: overriding callback for type"
                   << type;
    }

    GetJsonProcessors()[type] = processor;
}

void BaseCircuitItem::ConstructItemFromJson(RequiredItemMeta reqMeta,
                                            const json& itemMeta,
                                            QWidget* canvas)
{
    if (reqMeta.type == ItemType::Invalid)
    {
        qCritical()
            << "RegisterJsonProcessor: Can't register processor with invalid type";
        return;
    }

    if (!GetJsonProcessors().contains(reqMeta.type))
    {
        throw std::runtime_error("JSON processor is not registered for type "
                                 + std::to_string(reqMeta.type));
    }

    auto& processor = GetJsonProcessors().at(reqMeta.type);
    processor(reqMeta, itemMeta, canvas);
}

quint64 BaseCircuitItem::GetId() const
{
    return m_id;
}

std::vector<EndingConnector*> BaseCircuitItem::GetEndingConnectors() const
{
    return m_endingConnectors;
}

std::vector<StartingConnector*> BaseCircuitItem::GetStartingConnectors() const
{
    return m_startingConnectors;
}

void BaseCircuitItem::RemoveConnectionId(quint64 connId)
{
    for (auto* startingConnector : m_startingConnectors)
    {
        startingConnector->RemoveConnectionId(connId);
    }

    for (auto* endingConnector : m_endingConnectors)
    {
        endingConnector->RemoveConnectionId(connId);
    }
}

json BaseCircuitItem::GetJsonMeta() const
{
    json itemMeta;

    itemMeta["type"] = GetItemType();
    itemMeta["id"] = m_id;
    itemMeta["orderId"] = m_orderId;
    itemMeta["width"] = width();
    itemMeta["height"] = height();
    itemMeta["pos"]["x"] = pos().x();
    itemMeta["pos"]["y"] = pos().y();
    itemMeta["color"] = static_cast<quint64>(m_color.rgba64());

    auto& metaEndPoints = itemMeta["endPoints"];
    for (const auto* endingConnector : m_endingConnectors)
    {
        const auto connPos = endingConnector->GetEndPoint().connPos;
        json endPoint;
        endPoint["connPos"]["x"] = connPos.x();
        endPoint["connPos"]["y"] = connPos.y();

        metaEndPoints.push_back(endPoint);
    }

    auto& metaStartPoints = itemMeta["startPoints"];
    for (const auto* startingConnector : m_startingConnectors)
    {
        const auto connPos = startingConnector->GetStartPoint().connPos;
        json startPoint;
        startPoint["connPos"]["x"] = connPos.x();
        startPoint["connPos"]["y"] = connPos.y();

        metaStartPoints.push_back(startPoint);
    }

    return itemMeta;
}

void BaseCircuitItem::SetOrderId(int orderId)
{
    m_orderId = orderId;
    update();
}

void BaseCircuitItem::SetColor(const QColor& color)
{
    m_color = color;
    update();
}

void BaseCircuitItem::paintEvent(QPaintEvent* event)
{
    DrawToPixmap();

    QPainter painter(this);
    painter.drawPixmap(0, 0, m_pixmap);
}
