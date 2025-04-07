#include "CircuitElementMimeData.hpp"

CircuitElementMimeData::CircuitElementMimeData(QPoint eventPos)
    : BaseCircuitItemMimeData{eventPos}
{}

QDataStream& operator>>(QDataStream& iStream,
                        CircuitElementMimeData& data)
{
    data.readBasicMimeData(iStream);

    quint64 logicalVectorSize = 0;
    iStream >> logicalVectorSize;

    data.logicalVector.resize(logicalVectorSize);
    bool tmp;
    for (quint64 i = 0; i < logicalVectorSize; ++i)
    {
        iStream >> tmp;
        data.logicalVector[i] = tmp;
    }

    iStream >> data.isNotationBinary;

    return iStream;
}

QDataStream& operator<<(QDataStream& oStream,
                        const CircuitElementMimeData& data)
{
    data.writeBasicMimeData(oStream);

    quint64 logicalVectorSize = data.logicalVector.size();
    oStream << logicalVectorSize;

    for (quint64 i = 0; i < logicalVectorSize; ++i)
    {
        oStream << data.logicalVector[i];
    }

    oStream << data.isNotationBinary;

    return oStream;
}
