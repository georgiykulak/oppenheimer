#ifndef BASECIRCUITITEMMIMEDATA_HPP
#define BASECIRCUITITEMMIMEDATA_HPP

#include <ItemUtils.hpp>

#include <QPixmap>

struct BaseCircuitItemMimeData
{
    BaseCircuitItemMimeData(QPoint eventPos);

    EndingPointVector endingPoints;
    StartingPointVector startingPoints;
    std::vector<std::pair<QPoint, QPoint>> oldNewPoints;
    QPixmap pixmap;
    QColor color;
    QRect area;
    QSize itemSize;
    QPoint itemPosition;
    QPoint eventPos;
    QPoint offset = {};
    quint64 id = 0;
    int orderId = -1;
    bool value = false;

    QDataStream& readBasicMimeData(QDataStream& iStream);
    QDataStream& writeBasicMimeData(QDataStream& oStream) const;

private:
    QDataStream& readConnectors(QDataStream& iStream);
    QDataStream& writeConnectors(QDataStream& oStream) const;
};

#endif // BASECIRCUITITEMMIMEDATA_HPP
