#ifndef LOGICCONTROLLER_HPP
#define LOGICCONTROLLER_HPP

#include "items/Connection.hpp"
#include "presenters/PresenterTypes.hpp"

#include <unordered_map>

#include <QObject>

class AbstractItem;
class Simulator;

class LogicController : public QObject
{
    Q_OBJECT
public:
    explicit LogicController(QObject *parent = nullptr);

signals:
    void clearAllItems();
    void presentDHQTable(DHQTable dhqTable);

public slots:
    void AddNewInputItem(quint64 id);
    void AddNewOutputItem(quint64 id);
    void AddNewElementItem(quint64 id, std::size_t inputsSize);
    void RemoveItem(quint64 id);
    void ChangeElementItemInputsSize(quint64 id, std::size_t inputsSize);
    void ClearAllItems();
    void AddNewConnection(quint64 idStart, quint64 idEnd);
    void SetNumberParameterToElementItem(quint64 id, int numParam);
    void StartFunctionalFaultSimulation(quint64 elementId);

private:
    std::unordered_map<quint64, AbstractItem*> m_items;
    std::vector<Connection> m_conectionGraph;
    Simulator* m_simulator;
};

#endif // LOGICCONTROLLER_HPP
