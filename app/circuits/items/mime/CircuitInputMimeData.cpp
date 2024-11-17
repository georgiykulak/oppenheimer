#include "CircuitInputMimeData.hpp"

CircuitInputMimeData::CircuitInputMimeData(QPoint eventPos)
    : BaseCircuitItemMimeData{eventPos}
{}

QDataStream& operator>>(QDataStream& iStream,
                        CircuitInputMimeData& data)
{
    data.readBasicMimeData(iStream);
    return iStream;
}

QDataStream& operator<<(QDataStream& oStream,
                        const CircuitInputMimeData& data)
{
    data.writeBasicMimeData(oStream);
    return oStream;
}
