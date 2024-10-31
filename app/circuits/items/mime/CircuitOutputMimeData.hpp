#ifndef CIRCUITOUTPUTMIMEDATA_HPP
#define CIRCUITOUTPUTMIMEDATA_HPP

#include "BaseCircuitItemMimeData.hpp"

struct CircuitOutputMimeData : public BaseCircuitItemMimeData
{
    CircuitOutputMimeData(QPoint eventPos);

    QPoint oldEndPointPos;
    QPoint endOffset;
    quint64 connId;
    EndingPoint endPoint;
    QColor color;
};

QDataStream& operator>>(QDataStream& iStream, CircuitOutputMimeData& oData);

#endif // CIRCUITOUTPUTMIMEDATA_HPP
