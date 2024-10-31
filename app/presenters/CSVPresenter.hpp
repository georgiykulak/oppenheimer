#ifndef CSVPRESENTER_HPP
#define CSVPRESENTER_HPP

#include "PresenterTypes.hpp"

#include <QObject>
#include <QTextStream>

class CSVPresenter : public QObject
{
    Q_OBJECT
public:
    explicit CSVPresenter(QObject *parent = nullptr);

    void PresentDHQTable(const DHQTable& dhqTable);

private:
    std::size_t m_dhqCsvCounter = 1;

    void FormatDHQTable(QTextStream& stream, const DHQTable &dhqTable);
};

#endif // CSVPRESENTER_HPP
