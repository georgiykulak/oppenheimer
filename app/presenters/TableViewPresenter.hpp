#ifndef TABLEVIEWPRESENTER_HPP
#define TABLEVIEWPRESENTER_HPP

#include "PresenterTypes.hpp"

#include <QObject>

class TableViewPresenter : public QObject
{
    Q_OBJECT
public:
    explicit TableViewPresenter(QObject *parent = nullptr);

    void PresentDHQTable(const DHQTable& dhqTable);

private:
    QWidget* createHTableWidget(const DHQTable::DigitMatrix& hMatrix) const;
    QWidget* createQTableWidget(const DHQTable::BoolMatrix& qMatrix) const;
    QWidget* createDTableWidget(const DHQTable::BoolMatrix& dMatrix) const;
};

#endif // TABLEVIEWPRESENTER_HPP
