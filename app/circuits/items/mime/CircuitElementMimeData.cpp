#include "CircuitElementMimeData.hpp"

CircuitElementMimeData::CircuitElementMimeData(QPoint eventPos)
    : BaseCircuitItemMimeData{eventPos}
{}

QDataStream& operator>>(QDataStream& iStream,
                        CircuitElementMimeData& data)
{
    data.readBasicMimeData(iStream);

    iStream >> data.numberParam >> data.color >> data.isNotationBinary;

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
        data.endingPointVector.push_back({data.eventPos - endOffset,
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
        data.startingPointVector.push_back({data.eventPos - startOffset,
                                             startPoint.connIds});
    }

    for (unsigned int i = 0; i < endingPointVectorSize; ++i)
    {
        data.oldNewPoints.push_back({oldEndingPointVector[i].connPos,
                                      data.endingPointVector[i].connPos});
    }

    for (unsigned int i = 0; i < startingPointVectorSize; ++i)
    {
        data.oldNewPoints.push_back({oldStartingPointVector[i].connPos,
                                      data.startingPointVector[i].connPos});
    }

    return iStream;
}

QDataStream& operator<<(QDataStream& oStream,
                        const CircuitElementMimeData& data)
{
    data.writeBasicMimeData(oStream);

    oStream << data.numberParam << data.color << data.isNotationBinary;

    oStream << (unsigned int)(data.endingPointVector.size());
    for (const auto& endPoint : data.endingPointVector)
    {
        oStream
            << endPoint.connPos
            << QPoint(data.eventPos - endPoint.connPos)
            << endPoint.connId;
    }

    oStream << (unsigned int)(data.startingPointVector.size());
    for (const auto& startPoint : data.startingPointVector)
    {
        oStream
            << startPoint.connPos
            << QPoint(data.eventPos - startPoint.connPos);

        oStream << (unsigned int)(startPoint.connIds.size());
        for (const quint64 connId : startPoint.connIds)
        {
            oStream << connId;
        }
    }
}
