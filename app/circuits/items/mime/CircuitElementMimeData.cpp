#include "CircuitElementMimeData.hpp"

CircuitElementMimeData::CircuitElementMimeData(QPoint eventPos)
    : BaseCircuitItemMimeData{eventPos}
{}

QDataStream& CircuitElementMimeData::readMimeData(QDataStream& iStream)
{
    iStream
        >> numberParam
        >> isNotationBinary;

    return iStream;
}

QDataStream& operator>>(QDataStream& iStream,
                        CircuitElementMimeData& data)
{
    data.readBasicMimeData(iStream);
    data.readMimeData(iStream);

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
