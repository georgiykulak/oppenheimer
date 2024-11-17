#include "BaseCircuitItemMimeData.hpp"

BaseCircuitItemMimeData::BaseCircuitItemMimeData(QPoint eventPos)
    : eventPos{eventPos}
{}

QDataStream &BaseCircuitItemMimeData::readBasicMimeData(QDataStream& iStream)
{
    readConnectors(iStream);

    iStream
        >> pixmap
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
    unsigned int endingPointVectorSize;
    unsigned int startingPointVectorSize;

    iStream >> endingPointVectorSize;
    for (unsigned int i = 0; i < endingPointVectorSize; ++i)
    {
        EndingPoint endPoint;
        QPoint endOffset;

        iStream >> endPoint.connPos >> endOffset >> endPoint.connId;

        oldEndingPointVector.push_back(endPoint);
        endingPoints.push_back({eventPos - endOffset,
                                          endPoint.connId});
    }

    iStream >> startingPointVectorSize;
    for (unsigned int i = 0; i < startingPointVectorSize; ++i)
    {
        StartingPoint startPoint;
        QPoint startOffset;
        unsigned int connIdsSize;

        iStream >> startPoint.connPos >> startOffset >> connIdsSize;

        for (unsigned int j = 0; j < connIdsSize; ++j)
        {
            quint64 connId;
            iStream >> connId;
            startPoint.connIds.insert(connId);
        }

        oldStartingPointVector.push_back(startPoint);
        startingPoints.push_back({eventPos - startOffset,
                                            startPoint.connIds});
    }

    for (unsigned int i = 0; i < endingPointVectorSize; ++i)
    {
        oldNewPoints.push_back({oldEndingPointVector[i].connPos,
                                     endingPoints[i].connPos});
    }

    for (unsigned int i = 0; i < startingPointVectorSize; ++i)
    {
        oldNewPoints.push_back({oldStartingPointVector[i].connPos,
                                     startingPoints[i].connPos});
    }

    return iStream;
}

QDataStream &BaseCircuitItemMimeData::writeConnectors(QDataStream& oStream) const
{
    oStream << (unsigned int)(endingPoints.size());
    for (const auto& endPoint : endingPoints)
    {
        oStream
            << endPoint.connPos
            << QPoint(eventPos - endPoint.connPos)
            << endPoint.connId;
    }

    oStream << (unsigned int)(startingPoints.size());
    for (const auto& startPoint : startingPoints)
    {
        oStream
            << startPoint.connPos
            << QPoint(eventPos - startPoint.connPos);

        oStream << (unsigned int)(startPoint.connIds.size());
        for (const quint64 connId : startPoint.connIds)
        {
            oStream << connId;
        }
    }

    return oStream;
}
