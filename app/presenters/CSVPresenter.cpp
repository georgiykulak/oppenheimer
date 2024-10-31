#include "CSVPresenter.hpp"

#include <QDebug>
#include <QFile>

CSVPresenter::CSVPresenter(QObject *parent)
    : QObject{parent}
{

}

void CSVPresenter::PresentDHQTable(const DHQTable &dhqTable)
{
    QString csvFileName = "dhq_" + QString::number(m_dhqCsvCounter++) + ".csv";
    QFile csvFile(csvFileName);
    if (!csvFile.open(QIODevice::ReadWrite))
    {
        qDebug() << "CSVPresenter: can't open file" << csvFileName;
        return;
    }

    QTextStream stream(&csvFile);

    if (!dhqTable.truthTable.empty()
     && dhqTable.truthTable[0].size() == dhqTable.dMatrix.size()
     && dhqTable.dMatrix.size() == dhqTable.hMatrix.size()
     && dhqTable.hMatrix.size() == dhqTable.qMatrix.size()
     && dhqTable.qMatrix.size() == dhqTable.qVector.size())
    {
        FormatDHQTable(stream, dhqTable);
    }
    else
    {
        qDebug() << "CSVPresenter: D, H & Q matrix sizes are not equal!";
    }
    csvFile.close();
}

void CSVPresenter::FormatDHQTable(QTextStream &stream, const DHQTable &dhqTable)
{
    std::size_t csvLines = dhqTable.dMatrix.size();

    stream << ' ' << ',';

    const auto& truthTable = dhqTable.truthTable;
    const std::size_t ttRows = truthTable.size();
    const std::size_t ttColumns = 1 << ttRows;
    for (std::size_t j = 0; j < ttColumns; ++j)
    {
        QString ttLine;
        for (std::size_t i = 0; i < ttRows; ++i)
        {
            ttLine += QString::number(truthTable[ttRows - i - 1][j]);
        }
        stream << ttLine << ',';
    }

    for (std::size_t i = 0; i < dhqTable.hMatrix.size() + 3; ++i)
        stream << ' ' << ',';

    for (auto q : dhqTable.qVector)
        stream << QString::number(q) << ',';

    stream << Qt::endl;

    for (std::size_t l = 0; l < csvLines; ++l)
    {
        QString ttLine;
        for (std::size_t i = 0; i < ttRows; ++i)
        {
            ttLine += QString::number(truthTable[ttRows - i - 1][l]);
        }
        stream << ttLine << ',';

        for (auto d : dhqTable.dMatrix[l])
            stream << QString::number(d) << ',';

        stream << ' ' << ',';

        for (auto h : dhqTable.hMatrix[l])
            stream << QString::number(h) << ',';

        stream << ' ' << ',' << QString::number(dhqTable.qVector[l]) << ',';

        for (auto q : dhqTable.qMatrix[l])
            stream << QString::number(q) << ',';

        stream << Qt::endl;
    }
}
