#include "BaseCircuitItemMimeData.hpp"

BaseCircuitItemMimeData::BaseCircuitItemMimeData(QPoint eventPos)
    : eventPos{eventPos}
{}

QDataStream &BaseCircuitItemMimeData::readBasicMimeData(QDataStream &iStream)
{
    iStream
        >> pixmap
        >> offset
        >> id
        >> orderId
        >> itemSize
        >> value;

    itemPosition = QPoint(eventPos - offset);
    area = QRect(itemPosition, itemSize);

    return iStream;
}

QDataStream &BaseCircuitItemMimeData::writeBasicMimeData(QDataStream &oStream) const
{
    oStream
        << pixmap
        << offset
        << id
        << orderId
        << itemSize
        << value;

    return oStream;
}
