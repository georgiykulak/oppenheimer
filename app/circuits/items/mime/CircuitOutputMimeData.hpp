#ifndef CIRCUITOUTPUTMIMEDATA_HPP
#define CIRCUITOUTPUTMIMEDATA_HPP

#include "BaseCircuitItemMimeData.hpp"

struct CircuitOutputMimeData : public BaseCircuitItemMimeData
{
    CircuitOutputMimeData(QPoint eventPos = {});
};

QDataStream& operator>>(QDataStream& iStream,
                        CircuitOutputMimeData& data);
QDataStream& operator<<(QDataStream& oStream,
                        const CircuitOutputMimeData& data);

#endif // CIRCUITOUTPUTMIMEDATA_HPP
