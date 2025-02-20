#include "CircuitInputMimeData.hpp"

CircuitInputMimeData::CircuitInputMimeData(QPoint eventPos)
    : BaseCircuitItemMimeData{eventPos}
{}

QDataStream& operator>>(QDataStream& iStream, CircuitInputMimeData& iData)
{
    iData.readBasicMimeData(iStream);

    iStream
        >> iData.oldStartPointPos
        >> iData.startOffset
        >> iData.connIdsNumber
        >> iData.color;

    for (unsigned int i = 0; i < iData.connIdsNumber; ++i)
    {
        quint64 connId;
        iStream >> connId;
        iData.startPoint.connIds.insert(connId);
    }

    iData.startPoint.connPos = iData.eventPos - iData.startOffset;

    iData.oldNewPoints.emplace_back(iData.oldStartPointPos,
                                    iData.startPoint.connPos);

    return iStream;
}
