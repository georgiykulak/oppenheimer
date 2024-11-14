#ifndef BASECIRCUITITEMMIMEDATA_HPP
#define BASECIRCUITITEMMIMEDATA_HPP

#include <ItemUtils.hpp>

#include <QPixmap>

struct BaseCircuitItemMimeData
{
    BaseCircuitItemMimeData(QPoint eventPos);

    QPoint eventPos;
    QPixmap pixmap;
    QPoint offset = {};
    quint64 id = 0;
    QSize itemSize;
    int orderId = -1;
    bool value = false;
    QPoint itemPosition;
    QRect area;
    std::vector<std::pair<QPoint, QPoint>> oldNewPoints;

    QDataStream& readBasicMimeData(QDataStream& iStream);
    QDataStream& writeBasicMimeData(QDataStream& oStream) const;
};

#endif // BASECIRCUITITEMMIMEDATA_HPP
