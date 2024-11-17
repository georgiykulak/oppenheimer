#ifndef CIRCUITINPUTMIMEDATA_HPP
#define CIRCUITINPUTMIMEDATA_HPP

#include "BaseCircuitItemMimeData.hpp"

struct CircuitInputMimeData : public BaseCircuitItemMimeData
{
    CircuitInputMimeData(QPoint eventPos = {});
};

QDataStream& operator>>(QDataStream& iStream,
                        CircuitInputMimeData& data);
QDataStream& operator<<(QDataStream& oStream,
                        const CircuitInputMimeData& data);

#endif // CIRCUITINPUTMIMEDATA_HPP
