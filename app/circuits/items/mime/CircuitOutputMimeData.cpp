#include "CircuitOutputMimeData.hpp"

CircuitOutputMimeData::CircuitOutputMimeData(QPoint eventPos)
    : BaseCircuitItemMimeData{eventPos}
{}

QDataStream& operator>>(QDataStream& iStream, CircuitOutputMimeData& data)
{
    data.readBasicMimeData(iStream);

    iStream
        >> data.oldEndPointPos
        >> data.endOffset
        >> data.color
        >> data.connId;

    data.endPoint.connId = data.connId;
    data.endPoint.connPos = data.eventPos - data.endOffset;

    data.oldNewPoints.emplace_back(data.oldEndPointPos,
                                    data.endPoint.connPos);

    return iStream;
}

QDataStream& operator<<(QDataStream& oStream,
                        const CircuitOutputMimeData& data)
{
    data.writeBasicMimeData(oStream);

    oStream
        << data.oldEndPointPos
        << data.endOffset
        << data.color
        << data.connId;

    return oStream;
}
