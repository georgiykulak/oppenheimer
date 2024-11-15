#ifndef CIRCUITELEMENTMIMEDATA_HPP
#define CIRCUITELEMENTMIMEDATA_HPP

#include "BaseCircuitItemMimeData.hpp"

struct CircuitElementMimeData : public BaseCircuitItemMimeData
{
    CircuitElementMimeData(QPoint eventPos = {});

    EndingPointVector endingPointVector;
    StartingPointVector startingPointVector;

    int numberParam = 0;
    QColor color;
    bool isNotationBinary = true;
};

QDataStream& operator>>(QDataStream& iStream,
                        CircuitElementMimeData& eData);
QDataStream& operator<<(QDataStream& oStream,
                        const CircuitElementMimeData& data);

#endif // CIRCUITELEMENTMIMEDATA_HPP
