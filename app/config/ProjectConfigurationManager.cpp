#include "ProjectConfigurationManager.hpp"
#include "circuits/items/CircuitInput.hpp"
#include "circuits/items/CircuitOutput.hpp"
#include "circuits/items/CircuitElement.hpp"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

#include <fstream>

ProjectConfigurationManager::ProjectConfigurationManager(AreaManager& areaManager,
                                                         IdHandler& idHandler,
                                                         QObject* parent)
    : QObject{parent}
    , m_areaManager{areaManager}
    , m_idHandler{idHandler}
{}

void ProjectConfigurationManager::ResetCircuit()
{
    QObjectList childList = parent()->children();
    qDebug() << "CreateNewCircuit: childs:" << childList.size();
    // childList.size() == 3 means no items placed on canvas
    if (childList.size() > 3)
    {
        auto parentWidget = qobject_cast<QWidget*>(parent());
        if (!parentWidget)
        {
            return;
        }

        auto button = QMessageBox::question(parentWidget, tr("New Circuit"),
                                            tr("Do you want to save current circuit?"),
                                            QMessageBox::Yes,
                                            QMessageBox::No);

        if (button == QMessageBox::Yes)
        {
            SaveCircuitToFile();
        }
    }
    // Forget filename for saving
    m_fileName = "";

    emit clearCircuit();
}

void ProjectConfigurationManager::NewSavingFile()
{
    qDebug() << "NewSavingFile: Filename to reset:" << m_fileName;
    m_fileName = "";

    SaveCircuitToFile();
}

void ProjectConfigurationManager::SaveCircuitToFile()
{
    qDebug() << "Saving circuit to file:" << m_fileName;

    if (m_fileName.isEmpty())
    {
        auto parentWidget = qobject_cast<QWidget*>(parent());
        if (!parentWidget)
        {
            return;
        }

        m_fileName = QFileDialog::getSaveFileName(parentWidget, "Save Circuit As");
    }

    QFile file(m_fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    QObjectList childList = parent()->children();
    qDebug() << "Saving circuit, items:" << childList.size();

    json metaRoot;
    auto& metaItems = metaRoot["items"];
    auto& metaConnections = metaRoot["connections"];

    for (auto* obj : childList)
    {
        auto* item = qobject_cast<BaseCircuitItem*>(obj);
        if (item)
        {
            SaveCircuitItem(item, metaItems);
        }
    }

    SaveItemConnections(metaConnections);

    QTextStream out(&file);
    out << metaRoot.dump(4).c_str();
}

void ProjectConfigurationManager::OpenCircuitFromFile()
{
    auto parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget)
    {
        return;
    }

    auto fileName = QFileDialog::getOpenFileName(parentWidget, "Open Circuit");

    std::ifstream file(fileName.toStdString());
    if (!file.is_open())
    {
        return;
    }

    ResetCircuit();
    m_fileName = fileName;

    try
    {
        const json& metaRoot = json::parse(file);
        ConstructItemsFromJson(metaRoot);
    }
    catch (json::parse_error& err)
    {
        qWarning() << "OpenCircuitFromFile, caught parsing exception:"
                   << err.what();
        ResetCircuit();
        m_fileName = fileName;
    }
    catch (std::exception& err)
    {
        qWarning() << "OpenCircuitFromFile, caught unknown exception:"
                   << err.what();
        ResetCircuit();
        m_fileName = fileName;
    }
}

void ProjectConfigurationManager::SaveCircuitItem(BaseCircuitItem* item, json& metaItems)
{
    if (!item)
    {
        return;
    }

    json itemMeta;

    if (auto* circuitInput = qobject_cast<CircuitInput*>(item); circuitInput)
    {
        const auto mimeData = circuitInput->GetMimeData();

        itemMeta["type"] = circuitInput->GetItemType();
        itemMeta["id"] = mimeData.id;
        itemMeta["orderId"] = mimeData.orderId;
        itemMeta["width"] = mimeData.itemSize.width();
        itemMeta["height"] = mimeData.itemSize.height();
        itemMeta["pos"]["x"] = mimeData.itemPosition.x();
        itemMeta["pos"]["y"] = mimeData.itemPosition.y();
        itemMeta["color"] =
            static_cast<quint64>(mimeData.color.rgba64());

        auto& metaEndPoints = itemMeta["endPoints"];
        for (const auto& [connPos, _] : mimeData.endingPoints)
        {
            json endPoint;
            endPoint["connPos"]["x"] = connPos.x();
            endPoint["connPos"]["y"] = connPos.y();

            metaEndPoints.push_back(endPoint);
        }

        auto& metaStartPoints = itemMeta["startPoints"];
        for (const auto& [connPos, _] : mimeData.startingPoints)
        {
            json startPoint;
            startPoint["connPos"]["x"] = connPos.x();
            startPoint["connPos"]["y"] = connPos.y();

            metaStartPoints.push_back(startPoint);
        }
    }
    else if (auto* circuitOutput = qobject_cast<CircuitOutput*>(item); circuitOutput)
    {
        const auto mimeData = circuitOutput->GetMimeData();

        itemMeta["type"] = circuitOutput->GetItemType();
        itemMeta["id"] = mimeData.id;
        itemMeta["orderId"] = mimeData.orderId;
        itemMeta["width"] = mimeData.itemSize.width();
        itemMeta["height"] = mimeData.itemSize.height();
        itemMeta["pos"]["x"] = mimeData.itemPosition.x();
        itemMeta["pos"]["y"] = mimeData.itemPosition.y();
        itemMeta["color"] =
            static_cast<quint64>(mimeData.color.rgba64());

        auto& metaEndPoints = itemMeta["endPoints"];
        for (const auto& [connPos, _] : mimeData.endingPoints)
        {
            json endPoint;
            endPoint["connPos"]["x"] = connPos.x();
            endPoint["connPos"]["y"] = connPos.y();

            metaEndPoints.push_back(endPoint);
        }

        auto& metaStartPoints = itemMeta["startPoints"];
        for (const auto& [connPos, _] : mimeData.startingPoints)
        {
            json startPoint;
            startPoint["connPos"]["x"] = connPos.x();
            startPoint["connPos"]["y"] = connPos.y();

            metaStartPoints.push_back(startPoint);
        }
    }
    else if (auto* circuitElement = qobject_cast<CircuitElement*>(item); circuitElement)
    {
        const auto mimeData = circuitElement->GetMimeData();

        itemMeta["type"] = circuitElement->GetItemType();
        itemMeta["id"] = mimeData.id;
        itemMeta["orderId"] = mimeData.orderId;
        itemMeta["width"] = mimeData.itemSize.width();
        itemMeta["height"] = mimeData.itemSize.height();
        itemMeta["pos"]["x"] = mimeData.itemPosition.x();
        itemMeta["pos"]["y"] = mimeData.itemPosition.y();
        itemMeta["color"] =
            static_cast<quint64>(mimeData.color.rgba64());

        itemMeta["numberParam"] = mimeData.numberParam;
        itemMeta["isNotationBinary"] = mimeData.isNotationBinary;

        auto& metaEndPoints = itemMeta["endPoints"];
        for (const auto& [connPos, _] : mimeData.endingPoints)
        {
            json endPoint;
            endPoint["connPos"]["x"] = connPos.x();
            endPoint["connPos"]["y"] = connPos.y();

            metaEndPoints.push_back(endPoint);
        }

        auto& metaStartPoints = itemMeta["startPoints"];
        for (const auto& [connPos, _] : mimeData.startingPoints)
        {
            json startPoint;
            startPoint["connPos"]["x"] = connPos.x();
            startPoint["connPos"]["y"] = connPos.y();

            metaStartPoints.push_back(startPoint);
        }
    }
    else
    {
        throw std::runtime_error("ItemType is unknown, type = "
                                 + std::to_string(item->GetItemType()));
        return;
    }

    qDebug() << "Saving input item:" << itemMeta.dump().c_str();

    metaItems.push_back(itemMeta);
}

void ProjectConfigurationManager::SaveItemConnections(json& metaConnections)
{
    for (auto&& [_, conn] : m_areaManager.GetConnections())
    {
        auto&& [startId, endId, line] = conn;
        json connection;

        connection["startId"] = startId;
        connection["endId"] = endId;
        connection["startPos"]["x"] = line.p1().x();
        connection["startPos"]["y"] = line.p1().y();
        connection["endPos"]["x"] = line.p2().x();
        connection["endPos"]["y"] = line.p2().y();

        metaConnections.push_back(connection);
    }
}

void ProjectConfigurationManager::ConstructItemsFromJson(const json& metaRoot)
{
    qDebug() << "ConstructItemsFromJson";

    const auto& itemsArray = metaRoot.at("items");
    if (!itemsArray.is_null() && itemsArray.is_array())
    {
        qDebug() << "Items array is not empty, size =" << itemsArray.size();
        for (const auto& item : itemsArray)
        {
            auto itemType = item.at("type").template get<int>();
            switch (itemType)
            {
            case ItemType::Input:
            {
                ConstructInputItemFromJson(item);

                break;
            }
            case ItemType::Output:
            {
                ConstructOutputItemFromJson(item);

                break;
            }
            case ItemType::Element:
            {
                ConstructElementItemFromJson(item);

                break;
            }
            default:
            {
                continue;
            }
            }
        }
    }

    const auto& connectionsArray = metaRoot.at("connections");
    if (!connectionsArray.is_null() && connectionsArray.is_array())
    {
        qDebug() << "Connections array is not empty, size ="
                 << connectionsArray.size();
        for (const auto& connection : connectionsArray)
        {
            ConstructConnectionFromJson(connection);
        }
    }
}

void ProjectConfigurationManager::ConstructInputItemFromJson(const json& item)
{
    const auto& pos = item.at("pos");
    auto itemX = pos.at("x").template get<int>();
    auto itemY = pos.at("y").template get<int>();
    QPoint itemPosition(itemX, itemY);

    auto width = item.at("width").template get<int>();
    auto height = item.at("height").template get<int>();
    QSize itemSize(width, height);

    QRect area(itemPosition, itemSize);

    std::vector<std::pair<QPoint, QPoint>> emptyVector;

    if (!m_areaManager.TryBookArea(area, emptyVector))
    {
        return;
    }

    CircuitInputMimeData mimeData;

    mimeData.id = item.at("id").template get<quint64>();
    mimeData.orderId = item.at("orderId").template get<int>();

    auto orderIdInserted = m_idHandler.NewInputOrderId(mimeData.orderId);
    auto uidInserted = m_idHandler.InsertUid(mimeData.id);

    if (!orderIdInserted || !uidInserted)
    {
        return;
    }

    emit addNewInputItem(mimeData.id);

    quint64 rgba64Color = item.at("color").template get<quint64>();
    mimeData.color = QRgba64::fromRgba64(rgba64Color);

    const auto& endPoints = item.at("endPoints");
    if (!endPoints.is_null() && endPoints.is_array())
    {
        for (const auto& endPoint : endPoints)
        {
            const auto& connPos = endPoint.at("connPos");
            auto connX = connPos.at("x").template get<int>();
            auto connY = connPos.at("y").template get<int>();

            EndingPoint point;
            point.connPos = {connX, connY};
            mimeData.endingPoints.push_back(point);
        }
    }

    const auto& startPoints = item.at("startPoints");
    if (!startPoints.is_null() && startPoints.is_array())
    {
        for (const auto& startPoint : startPoints)
        {
            const auto& connPos = startPoint.at("connPos");
            auto connX = connPos.at("x").template get<int>();
            auto connY = connPos.at("y").template get<int>();

            StartingPoint point;
            point.connPos = {connX, connY};
            mimeData.startingPoints.push_back(point);
        }
    }

    auto parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget)
    {
        return;
    }

    auto* circuitItem = new CircuitInput(mimeData, parentWidget);
    circuitItem->move(itemPosition);
}

void ProjectConfigurationManager::ConstructOutputItemFromJson(const json& item)
{
    const auto& pos = item.at("pos");
    auto itemX = pos.at("x").template get<int>();
    auto itemY = pos.at("y").template get<int>();
    QPoint itemPosition(itemX, itemY);

    auto width = item.at("width").template get<int>();
    auto height = item.at("height").template get<int>();
    QSize itemSize(width, height);

    QRect area(itemPosition, itemSize);

    std::vector<std::pair<QPoint, QPoint>> emptyVector;

    if (!m_areaManager.TryBookArea(area, emptyVector))
    {
        return;
    }

    CircuitOutputMimeData mimeData;

    mimeData.id = item.at("id").template get<quint64>();
    mimeData.orderId = item.at("orderId").template get<int>();

    auto orderIdInserted = m_idHandler.NewOutputOrderId(mimeData.orderId);
    auto uidInserted = m_idHandler.InsertUid(mimeData.id);

    if (!orderIdInserted || !uidInserted)
    {
        return;
    }

    emit addNewOutputItem(mimeData.id);

    quint64 rgba64Color = item.at("color").template get<quint64>();
    mimeData.color = QRgba64::fromRgba64(rgba64Color);

    const auto& endPoints = item.at("endPoints");
    if (!endPoints.is_null() && endPoints.is_array())
    {
        for (const auto& endPoint : endPoints)
        {
            const auto& connPos = endPoint.at("connPos");
            auto connX = connPos.at("x").template get<int>();
            auto connY = connPos.at("y").template get<int>();

            EndingPoint point;
            point.connPos = {connX, connY};
            mimeData.endingPoints.push_back(point);
        }
    }

    const auto& startPoints = item.at("startPoints");
    if (!startPoints.is_null() && startPoints.is_array())
    {
        for (const auto& startPoint : startPoints)
        {
            const auto& connPos = startPoint.at("connPos");
            auto connX = connPos.at("x").template get<int>();
            auto connY = connPos.at("y").template get<int>();

            StartingPoint point;
            point.connPos = {connX, connY};
            mimeData.startingPoints.push_back(point);
        }
    }

    auto parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget)
    {
        return;
    }

    auto* circuitItem = new CircuitOutput(mimeData, parentWidget);
    circuitItem->move(itemPosition);
}

void ProjectConfigurationManager::ConstructElementItemFromJson(const json& item)
{
    const auto& pos = item.at("pos");
    auto itemX = pos.at("x").template get<int>();
    auto itemY = pos.at("y").template get<int>();
    QPoint itemPosition(itemX, itemY);

    auto width = item.at("width").template get<int>();
    auto height = item.at("height").template get<int>();
    QSize itemSize(width, height);

    QRect area(itemPosition, itemSize);

    std::vector<std::pair<QPoint, QPoint>> emptyVector;

    if (!m_areaManager.TryBookArea(area, emptyVector))
    {
        return;
    }

    CircuitElementMimeData mimeData;

    mimeData.id = item.at("id").template get<quint64>();
    mimeData.orderId = item.at("orderId").template get<int>();

    auto orderIdInserted = m_idHandler.NewElementOrderId(mimeData.orderId);
    auto uidInserted = m_idHandler.InsertUid(mimeData.id);

    if (!orderIdInserted || !uidInserted)
    {
        return;
    }

    emit addNewElementItem(mimeData.id, mimeData.endingPoints.size());

    quint64 rgba64Color = item.at("color").template get<quint64>();
    mimeData.color = QRgba64::fromRgba64(rgba64Color);

    mimeData.numberParam = item.at("numberParam").template get<int>();
    mimeData.isNotationBinary = item.at("isNotationBinary").template get<bool>();

    const auto& endPoints = item.at("endPoints");
    if (!endPoints.is_null() && endPoints.is_array())
    {
        for (const auto& endPoint : endPoints)
        {
            const auto& connPos = endPoint.at("connPos");
            auto connX = connPos.at("x").template get<int>();
            auto connY = connPos.at("y").template get<int>();

            EndingPoint point;
            point.connPos = {connX, connY};
            mimeData.endingPoints.push_back(point);
        }
    }

    const auto& startPoints = item.at("startPoints");
    if (!startPoints.is_null() && startPoints.is_array())
    {
        for (const auto& startPoint : startPoints)
        {
            const auto& connPos = startPoint.at("connPos");
            auto connX = connPos.at("x").template get<int>();
            auto connY = connPos.at("y").template get<int>();

            StartingPoint point;
            point.connPos = {connX, connY};
            mimeData.startingPoints.push_back(point);
        }
    }

    auto parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget)
    {
        return;
    }

    auto* circuitItem = new CircuitElement(mimeData, parentWidget);
    circuitItem->move(itemPosition);
}

void ProjectConfigurationManager::ConstructConnectionFromJson(const json& connection)
{
    const auto startId = connection.at("startId").template get<quint64>();
    const auto endId = connection.at("endId").template get<quint64>();

    const auto startPos = connection.at("startPos");
    auto startX = startPos.at("x").template get<int>();
    auto startY = startPos.at("y").template get<int>();
    QPoint startPosPoint = {startX, startY};

    const auto endPos = connection.at("endPos");
    auto endX = endPos.at("x").template get<int>();
    auto endY = endPos.at("y").template get<int>();
    QPoint endPosPoint = {endX, endY};

    const auto positions = QLine(startPosPoint, endPosPoint);

    emit insertConnection(startId, endId, positions);
}
