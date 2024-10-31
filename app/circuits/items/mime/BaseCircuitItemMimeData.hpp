#ifndef BASECIRCUITITEMMIMEDATA_HPP
#define BASECIRCUITITEMMIMEDATA_HPP

#include <ItemUtils.hpp>

#include <QPixmap>

struct BaseCircuitItemMimeData
{
    BaseCircuitItemMimeData(QPoint eventPos);

    QPoint eventPos;
    QPixmap pixmap;
    QPoint offset;
    quint64 id;
    QSize itemSize;
    int orderId;
    bool value;
    QPoint itemPosition;
    QRect area;
    std::vector<std::pair<QPoint, QPoint>> oldNewPoints;

    QDataStream& readBasicMimeData(QDataStream& iStream);
};

#endif // BASECIRCUITITEMMIMEDATA_HPP
