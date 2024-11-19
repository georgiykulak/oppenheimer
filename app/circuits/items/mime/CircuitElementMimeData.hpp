#ifndef CIRCUITELEMENTMIMEDATA_HPP
#define CIRCUITELEMENTMIMEDATA_HPP

#include "BaseCircuitItemMimeData.hpp"

struct CircuitElementMimeData : public BaseCircuitItemMimeData
{
    CircuitElementMimeData(QPoint eventPos = {});

    int numberParam = 0;
    bool isNotationBinary = true;

    QDataStream& readMimeData(QDataStream& iStream);
};

QDataStream& operator>>(QDataStream& iStream,
                        CircuitElementMimeData& eData);
QDataStream& operator<<(QDataStream& oStream,
                        const CircuitElementMimeData& data);

#endif // CIRCUITELEMENTMIMEDATA_HPP
