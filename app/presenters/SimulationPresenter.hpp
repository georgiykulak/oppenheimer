#ifndef SIMULATIONPRESENTER_HPP
#define SIMULATIONPRESENTER_HPP

#include "PresenterTypes.hpp"

#include <QObject>

class CSVPresenter;
class TableViewPresenter;

class SimulationPresenter : public QObject
{
    Q_OBJECT
public:
    explicit SimulationPresenter(QObject *parent = nullptr);

public slots:
    void PresentDHQTable(DHQTable dhqTable);

private:
    CSVPresenter* m_csvPresenter;
    TableViewPresenter* m_tableViewPresenter;
};

#endif // SIMULATIONPRESENTER_HPP
