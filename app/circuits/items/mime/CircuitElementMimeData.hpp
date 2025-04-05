#ifndef CIRCUITELEMENTMIMEDATA_HPP
#define CIRCUITELEMENTMIMEDATA_HPP

#include "BaseCircuitItemMimeData.hpp"

struct CircuitElementMimeData : public BaseCircuitItemMimeData
{
    CircuitElementMimeData(QPoint eventPos = {});

    std::vector<bool> logicalVector;
    bool isNotationBinary = true;
};

QDataStream& operator>>(QDataStream& iStream,
                        CircuitElementMimeData& eData);
QDataStream& operator<<(QDataStream& oStream,
                        const CircuitElementMimeData& data);

#endif // CIRCUITELEMENTMIMEDATA_HPP
