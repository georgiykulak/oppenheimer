#include "BaseCircuitItemMimeData.hpp"

#include <QDebug>

BaseCircuitItemMimeData::BaseCircuitItemMimeData(QPoint eventPos)
    : eventPos{eventPos}
{}

QDataStream &BaseCircuitItemMimeData::readBasicMimeData(QDataStream& iStream)
{
    readConnectors(iStream);

    iStream
        >> pixmap
        >> color
        >> offset
        >> id
        >> orderId
        >> itemSize
        >> value;

    itemPosition = QPoint(eventPos - offset);
    area = QRect(itemPosition, itemSize);

    return iStream;
}

QDataStream &BaseCircuitItemMimeData::writeBasicMimeData(QDataStream& oStream) const
{
    writeConnectors(oStream);

    oStream
        << pixmap
        << color
        << offset
        << id
        << orderId
        << itemSize
        << value;

    return oStream;
}

QDataStream &BaseCircuitItemMimeData::readConnectors(QDataStream& iStream)
{
    EndingPointVector oldEndingPointVector;
    StartingPointVector oldStartingPointVector;
    quint64 endingPointVectorSize;
    quint64 startingPointVectorSize;

    iStream >> endingPointVectorSize;
    for (quint64 i = 0; i < endingPointVectorSize; ++i)
    {
        EndingPoint endPoint;
        QPoint endOffset;

        iStream >> endPoint.connPos >> endOffset >> endPoint.connId;

        oldEndingPointVector.push_back(endPoint);
        endingPoints.push_back({eventPos - endOffset,
                                          endPoint.connId});
    }

    iStream >> startingPointVectorSize;
    for (quint64 i = 0; i < startingPointVectorSize; ++i)
    {
        StartingPoint startPoint;
        QPoint startOffset;
        quint64 connIdsSize;

        iStream >> startPoint.connPos >> startOffset >> connIdsSize;

        for (quint64 j = 0; j < connIdsSize; ++j)
        {
            quint64 connId;
            iStream >> connId;
            startPoint.connIds.insert(connId);
        }

        oldStartingPointVector.push_back(startPoint);
        startingPoints.push_back({eventPos - startOffset,
                                            startPoint.connIds});
    }

    for (quint64 i = 0; i < endingPointVectorSize; ++i)
    {
        oldNewPoints.push_back({oldEndingPointVector[i].connPos,
                                     endingPoints[i].connPos});
    }

    for (quint64 i = 0; i < startingPointVectorSize; ++i)
    {
        oldNewPoints.push_back({oldStartingPointVector[i].connPos,
                                     startingPoints[i].connPos});
    }

    return iStream;
}

QDataStream &BaseCircuitItemMimeData::writeConnectors(QDataStream& oStream) const
{
    oStream << static_cast<quint64>(endingPoints.size());
    for (const auto& endPoint : endingPoints)
    {
        oStream
            << endPoint.connPos
            << QPoint(eventPos - endPoint.connPos)
            << endPoint.connId;
    }

    oStream << static_cast<quint64>(startingPoints.size());
    for (const auto& startPoint : startingPoints)
    {
        oStream
            << startPoint.connPos
            << QPoint(eventPos - startPoint.connPos);

        oStream << static_cast<quint64>(startPoint.connIds.size());
        for (const quint64 connId : startPoint.connIds)
        {
            oStream << connId;
        }
    }

    return oStream;
}
