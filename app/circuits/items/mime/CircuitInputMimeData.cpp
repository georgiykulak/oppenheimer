#include "CircuitInputMimeData.hpp"

CircuitInputMimeData::CircuitInputMimeData(QPoint eventPos)
    : BaseCircuitItemMimeData{eventPos}
{}

QDataStream& operator>>(QDataStream& iStream,
                        CircuitInputMimeData& data)
{
    data.readBasicMimeData(iStream);

    iStream
        >> data.color;

    return iStream;
}

QDataStream& operator<<(QDataStream& oStream,
                        const CircuitInputMimeData& data)
{
    data.writeBasicMimeData(oStream);

    oStream
        << data.color;

    return oStream;
}
