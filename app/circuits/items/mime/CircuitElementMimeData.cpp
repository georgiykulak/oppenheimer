#include "CircuitElementMimeData.hpp"

CircuitElementMimeData::CircuitElementMimeData(QPoint eventPos)
    : BaseCircuitItemMimeData{eventPos}
{}

QDataStream& operator>>(QDataStream& iStream,
                        CircuitElementMimeData& data)
{
    data.readBasicMimeData(iStream);

    iStream
        >> data.numberParam
        >> data.isNotationBinary;

    return iStream;
}

QDataStream& operator<<(QDataStream& oStream,
                        const CircuitElementMimeData& data)
{
    data.writeBasicMimeData(oStream);

    oStream
        << data.numberParam
        << data.isNotationBinary;

    return oStream;
}
