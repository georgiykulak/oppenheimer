#ifndef CIRCUITELEMENTMIMEDATA_HPP
#define CIRCUITELEMENTMIMEDATA_HPP

#include "BaseCircuitItemMimeData.hpp"

struct CircuitElementMimeData : public BaseCircuitItemMimeData
{
    CircuitElementMimeData(QPoint eventPos);

    unsigned int endingPointVectorSize;
    EndingPointVector endingPointVector;
    StartingPointVector startingPointVector;

    int numberParam;
    QColor color;
    bool isNotationBinary;
};

QDataStream& operator>>(QDataStream& iStream, CircuitElementMimeData& eData);

#endif // CIRCUITELEMENTMIMEDATA_HPP
