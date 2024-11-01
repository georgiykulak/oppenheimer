#include "LogicController.hpp"
#include "items/InputItem.hpp"
#include "items/OutputItem.hpp"
#include "items/ElementItem.hpp"
#include "items/Connection.hpp"
#include "simulation/Simulator.hpp"

#include <QDebug>

LogicController::LogicController(QObject *parent)
    : QObject{parent}
{
    m_simulator = new Simulator;
}

void LogicController::AddNewInputItem(quint64 id)
{
    AbstractItem* item = new InputItem(this);
    m_items.insert(std::make_pair(id, item));

    qDebug() << "AddNewInputItem: size =" << m_items.size() << "id =" << id
             << "type =" << m_items.at(id)->GetItemType();
}

void LogicController::AddNewOutputItem(quint64 id)
{
    AbstractItem* item = new OutputItem(this);
    m_items.insert(std::make_pair(id, item));

    qDebug() << "AddNewOutputItem: size =" << m_items.size() << "id =" << id
             << "type =" << m_items.at(id)->GetItemType();
}

void LogicController::AddNewElementItem(quint64 id, std::size_t inputsSize)
{
    AbstractItem* item = new ElementItem(this);
    item->SetInputsSize(inputsSize);
    m_items.insert(std::make_pair(id, item));

    qDebug() << "AddNewElementItem: size =" << m_items.size() << "id =" << id
             << "type =" << m_items.at(id)->GetItemType();
}

void LogicController::RemoveItem(quint64 id)
{
    if (!m_items.contains(id))
    {
        return;
    }

    qDebug() << "RemoveItem: size =" << m_items.size() << "id =" << id
             << "type =" << m_items.at(id)->GetItemType();
    m_items.erase(id);
}

void LogicController::ChangeElementItemInputsSize(quint64 id, std::size_t inputsSize)
{
    qDebug() << "Change element input size: size =" << m_items.size() << "id =" << id
             << "type =" << m_items.at(id)->GetItemType() << "inputs size =" << inputsSize;

    auto element = m_items.find(id);
    if (element != m_items.end()
     && element->second->GetItemType() == ItemType::Element
     && element->second->GetInputsSize() != inputsSize)
    {
        element->second->SetInputsSize(inputsSize);
        element->second->SetNumParam(0);
    }
}

void LogicController::ClearAllItems()
{
    qDebug() << "Clear" << m_items.size() << "items";

    for (const auto& [id, item] : m_items)
    {
        delete m_items[id];
        m_items[id] = nullptr;
    }

    m_items.clear();

    emit clearAllItems();
}

void LogicController::AddNewConnection(quint64 idStart, quint64 idEnd)
{
    Connection connection;
    connection.m_startId = idStart;
    connection.m_endId = idEnd;
    m_conectionGraph.push_back(connection);
}

void LogicController::SetNumberParameterToElementItem(quint64 id, int numParam)
{
    qDebug() << "Set number parameter: size =" << m_items.size() << "id =" << id
             << "type =" << m_items.at(id)->GetItemType() << "numParam =" << numParam;

    auto element = m_items.find(id);
    if (element != m_items.end()
     && element->second->GetItemType() == ItemType::Element)
    {
        element->second->SetNumParam(numParam);
    }
}

void LogicController::StartFunctionalFaultSimulation(quint64 elementId)
{
    auto element = m_items.find(elementId);
    if (element == m_items.end()
     || element->second->GetItemType() != ItemType::Element)
    {
        qDebug() << "LogicController: Can't start functional fault simulation - "
                    "no element was found with id =" << elementId;

        return;
    }

    qDebug() << "LogicController: Starting functional fault simulation for "
                "element with id =" << elementId;

    const auto qNumber = element->second->GetNumParam();
    const auto inputsSize = element->second->GetInputsSize();
    const auto inputVectorSize = 1 << inputsSize; // 2 ^ N, N = inputsSize

    DHQTable dhqTable;

    m_simulator->SimulateFunctionalFault(inputVectorSize,
                                         qNumber,
                                         dhqTable.qMatrix,
                                         dhqTable.hMatrix,
                                         dhqTable.dMatrix,
                                         dhqTable.truthTable,
                                         dhqTable.qVector);

    emit presentDHQTable(dhqTable);
}
