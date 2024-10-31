#include "CircuitElementMimeData.hpp"

CircuitElementMimeData::CircuitElementMimeData(QPoint eventPos)
    : BaseCircuitItemMimeData{eventPos}
{}

QDataStream& operator>>(QDataStream& iStream, CircuitElementMimeData& eData)
{
    eData.readBasicMimeData(iStream);

    iStream >> eData.numberParam >> eData.color >> eData.isNotationBinary;

    EndingPointVector oldEndingPointVector;
    StartingPointVector oldStartingPointVector;
    unsigned int startingPointVectorSize;

    iStream >> eData.endingPointVectorSize;
    for (unsigned int i = 0; i < eData.endingPointVectorSize; ++i)
    {
        EndingPoint endPoint;
        QPoint endOffset;

        iStream >> endPoint.connPos >> endOffset >> endPoint.connId;

        oldEndingPointVector.push_back(endPoint);
        eData.endingPointVector.push_back({eData.eventPos - endOffset,
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
        eData.startingPointVector.push_back({eData.eventPos - startOffset,
                                             startPoint.connIds});
    }

    for (unsigned int i = 0; i < eData.endingPointVectorSize; ++i)
    {
        eData.oldNewPoints.push_back({oldEndingPointVector[i].connPos,
                                      eData.endingPointVector[i].connPos});
    }

    for (unsigned int i = 0; i < startingPointVectorSize; ++i)
    {
        eData.oldNewPoints.push_back({oldStartingPointVector[i].connPos,
                                      eData.startingPointVector[i].connPos});
    }

    return iStream;
}
