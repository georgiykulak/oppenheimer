#include "CircuitOutputMimeData.hpp"

CircuitOutputMimeData::CircuitOutputMimeData(QPoint eventPos)
    : BaseCircuitItemMimeData{eventPos}
{}

QDataStream& operator>>(QDataStream& iStream, CircuitOutputMimeData& data)
{
    data.readBasicMimeData(iStream);
    return iStream;
}

QDataStream& operator<<(QDataStream& oStream,
                        const CircuitOutputMimeData& data)
{
    data.writeBasicMimeData(oStream);
    return oStream;
}
