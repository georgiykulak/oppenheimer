#include "SimulationPresenter.hpp"
#include "CSVPresenter.hpp"
#include "TableViewPresenter.hpp"

#include <QDebug>

SimulationPresenter::SimulationPresenter(QObject *parent)
    : QObject{parent}
{
    m_csvPresenter = new CSVPresenter(this);
    m_tableViewPresenter = new TableViewPresenter(this);
}

void SimulationPresenter::PresentDHQTable(DHQTable dhqTable)
{
    qDebug() << "SimulationPresenter: qMatrix size =" << dhqTable.qMatrix.size();
    qDebug() << "SimulationPresenter: hMatrix size =" << dhqTable.hMatrix.size();
    qDebug() << "SimulationPresenter: dMatrix size =" << dhqTable.dMatrix.size();
    qDebug() << "SimulationPresenter: truthTable size =" << dhqTable.truthTable.size();

    m_csvPresenter->PresentDHQTable(dhqTable);
    m_tableViewPresenter->PresentDHQTable(dhqTable);
}
