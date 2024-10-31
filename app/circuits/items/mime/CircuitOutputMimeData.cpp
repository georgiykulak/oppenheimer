#include "CircuitOutputMimeData.hpp"

CircuitOutputMimeData::CircuitOutputMimeData(QPoint eventPos)
    : BaseCircuitItemMimeData{eventPos}
{}

QDataStream& operator>>(QDataStream& iStream, CircuitOutputMimeData& oData)
{
    oData.readBasicMimeData(iStream);

    iStream
        >> oData.oldEndPointPos
        >> oData.endOffset
        >> oData.color
        >> oData.connId;

    oData.endPoint.connId = oData.connId;
    oData.endPoint.connPos = oData.eventPos - oData.endOffset;

    oData.oldNewPoints.emplace_back(oData.oldEndPointPos,
                                    oData.endPoint.connPos);

    return iStream;
}
