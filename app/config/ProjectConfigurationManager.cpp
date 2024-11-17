#include "ProjectConfigurationManager.hpp"
#include "circuits/items/BaseCircuitItem.hpp"

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
    // childList.size() == 4 means no items placed on canvas
    if (childList.size() > 4)
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
        ConstructCircuitFromJson(metaRoot);
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

    json itemMeta = item->GetJsonMeta();

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

void ProjectConfigurationManager::ConstructCircuitFromJson(const json& metaRoot)
{
    qDebug() << "ConstructCircuitFromJson";

    const auto& itemsArray = metaRoot.at("items");
    if (!itemsArray.is_null() && itemsArray.is_array())
    {
        qDebug() << "Items array is not empty, size =" << itemsArray.size();
        for (const auto& item : itemsArray)
        {
            ConstructItemFromJson(item);
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

void ProjectConfigurationManager::ConstructItemFromJson(const json& itemMeta)
{
    RequiredItemMeta meta;

    meta.type = itemMeta.at("type").template get<quint64>();
    const auto& pos = itemMeta.at("pos");
    const auto itemX = pos.at("x").template get<int>();
    const auto itemY = pos.at("y").template get<int>();
    meta.itemPosition = QPoint(itemX, itemY);

    auto width = itemMeta.at("width").template get<int>();
    auto height = itemMeta.at("height").template get<int>();
    meta.itemSize = QSize(width, height);

    QRect area(meta.itemPosition, meta.itemSize);

    std::vector<std::pair<QPoint, QPoint>> emptyVector;

    if (!m_areaManager.TryBookArea(area, emptyVector))
    {
        return;
    }

    meta.id = itemMeta.at("id").template get<quint64>();
    meta.orderId = itemMeta.at("orderId").template get<int>();

    auto orderIdInserted = m_idHandler.NewOrderId(meta.type, meta.orderId);
    auto uidInserted = m_idHandler.InsertUid(meta.id);

    if (!orderIdInserted || !uidInserted)
    {
        m_areaManager.ClearArea(area);
        return;
    }

    if (itemMeta.contains("color"))
    {
        quint64 rgba64Color = itemMeta.at("color").template get<quint64>();
        meta.color = QRgba64::fromRgba64(rgba64Color);
    }

    const auto& endPoints = itemMeta.at("endPoints");
    if (!endPoints.is_null() && endPoints.is_array())
    {
        for (const auto& endPoint : endPoints)
        {
            const auto& connPos = endPoint.at("connPos");
            auto connX = connPos.at("x").template get<int>();
            auto connY = connPos.at("y").template get<int>();

            EndingPoint point;
            point.connPos = {connX, connY};
            meta.endingPoints.push_back(point);
        }
    }

    const auto& startPoints = itemMeta.at("startPoints");
    if (!startPoints.is_null() && startPoints.is_array())
    {
        for (const auto& startPoint : startPoints)
        {
            const auto& connPos = startPoint.at("connPos");
            auto connX = connPos.at("x").template get<int>();
            auto connY = connPos.at("y").template get<int>();

            StartingPoint point;
            point.connPos = {connX, connY};
            meta.startingPoints.push_back(point);
        }
    }

    emit addNewItem(meta.type, meta.id, meta.endingPoints.size());

    auto parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget)
    {
        return;
    }

    BaseCircuitItem::ConstructItemFromJson(meta,
                                           itemMeta,
                                           parentWidget);
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
