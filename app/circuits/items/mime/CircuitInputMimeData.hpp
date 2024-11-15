#ifndef CIRCUITINPUTMIMEDATA_HPP
#define CIRCUITINPUTMIMEDATA_HPP

#include "BaseCircuitItemMimeData.hpp"

struct CircuitInputMimeData : public BaseCircuitItemMimeData
{
    CircuitInputMimeData(QPoint eventPos = {});

    QPoint oldStartPointPos;
    QPoint startOffset;
    unsigned int connIdsNumber;
    StartingPoint startPoint;
    QColor color;
};

QDataStream& operator>>(QDataStream& iStream,
                        CircuitInputMimeData& data);
QDataStream& operator<<(QDataStream& oStream,
                        const CircuitInputMimeData& data);

#endif // CIRCUITINPUTMIMEDATA_HPP
