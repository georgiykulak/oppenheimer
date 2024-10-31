#ifndef CIRCUITINPUTMIMEDATA_HPP
#define CIRCUITINPUTMIMEDATA_HPP

#include "BaseCircuitItemMimeData.hpp"

struct CircuitInputMimeData : public BaseCircuitItemMimeData
{
    CircuitInputMimeData(QPoint eventPos);

    QPoint oldStartPointPos;
    QPoint startOffset;
    unsigned int connIdsNumber;
    StartingPoint startPoint;
    QColor color;
};

QDataStream& operator>>(QDataStream& iStream, CircuitInputMimeData& iData);

#endif // CIRCUITINPUTMIMEDATA_HPP
