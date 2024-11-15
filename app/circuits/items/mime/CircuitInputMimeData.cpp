#include "CircuitInputMimeData.hpp"

CircuitInputMimeData::CircuitInputMimeData(QPoint eventPos)
    : BaseCircuitItemMimeData{eventPos}
{}

QDataStream& operator>>(QDataStream& iStream,
                        CircuitInputMimeData& data)
{
    data.readBasicMimeData(iStream);

    iStream
        >> data.oldStartPointPos
        >> data.startOffset
        >> data.connIdsNumber
        >> data.color;

    for (unsigned int i = 0; i < data.connIdsNumber; ++i)
    {
        quint64 connId;
        iStream >> connId;
        data.startPoint.connIds.insert(connId);
    }

    data.startPoint.connPos = data.eventPos - data.startOffset;

    data.oldNewPoints.emplace_back(data.oldStartPointPos,
                                    data.startPoint.connPos);

    return iStream;
}

QDataStream& operator<<(QDataStream& oStream,
                        const CircuitInputMimeData& data)
{
    data.writeBasicMimeData(oStream);

    oStream
        << data.oldStartPointPos
        << data.startOffset
        << data.connIdsNumber
        << data.color;

    for (const quint64 connId : data.startPoint.connIds)
    {
        oStream << connId;
    }

    return oStream;
}
