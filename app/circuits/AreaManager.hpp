#ifndef AREAMANAGER_H
#define AREAMANAGER_H

#include <Config.hpp>
#include "graph/CommonTypes.hpp"

#include <QPolygon>
#include <QLine>

class DndBackup;
class GraphDesigner;

class AreaManager : public QObject
{
    Q_OBJECT
public:
    struct Connection
    {
        quint64 startId;
        quint64 endId;
        QLine line;
    };

    explicit AreaManager(QObject *parent = nullptr);

    void SetMatrixSize(QSize windowSize);
    bool TryBookArea(const QRect& region,
                     const std::vector<std::pair<QPoint, QPoint>>& oldNewPoints);
    void ClearArea(const QRect& region);
    void ClearAndBackupArea(const QRect& region);
    void ClearMatrix();

    std::size_t ConnectionSize() const;
    quint64 AddConnection(const QLine& line,
                          quint64 startId,
                          quint64 endId);
    void RemoveConnection(quint64 connId);
    std::vector<QPolygon> GetConnectionPolygons() const;
    void RemoveAllConnections();
    std::unordered_map<quint64, Connection>
    GetConnections() const { return m_connections; }

private:
    StatusMatrix m_matrix;

    std::unordered_map<quint64, IndexVector> m_edges;
    std::unordered_map<quint64, Connection> m_connections;
    std::unordered_map<quint64, std::pair<QPoint, QPoint>> m_curvePoints;

    const int m_cellSize;

    DndBackup* m_dndBackup;
    GraphDesigner* m_graphDesigner;

    bool UpdateConnection(quint64 connId,
                          IndexVector& edge,
                          QPoint& srcCurvePoint,
                          QPoint& dstCurvePoint);
    bool UpdateConnections(
        std::unordered_map<quint64, Connection>& updConnPoints,
        std::unordered_map<quint64, IndexVector>& updatedEdges,
        std::unordered_map<quint64, std::pair<QPoint, QPoint>>& updatedCurvePoints,
        QPoint oldPoint,
        QPoint newPoint);
    void FreeConnection(const IndexVector& edge,
                        std::function<void (BookStatus&)> handleFreeStatus);
    bool FindSubmatrix(
        const QRect& region,
        IndexRegion& idxReg,
        std::function<bool (BookStatus, IndexPoint)> handleBookStatus);
    bool CreateClosestStartPoint(IndexVector& edge,
                                 IndexPoint& src,
                                 QPoint& srcCurvePoint,
                                 QPoint start);
    bool CreateClosestEndPoint(IndexVector& edgeEnd,
                               IndexPoint& dst,
                               QPoint& dstCurvePoint,
                               QPoint end);
    bool DesignEdge(IndexVector& edge,
                    QPoint& srcCurvePoint,
                    QPoint& dstCurvePoint,
                    QPoint start,
                    QPoint end);

#ifdef VISUAL_DEBUGGING
private:
    // For debug purposes
    std::vector<QLine> m_net;
    void SetNetLines(int width, int height);

public:
    const std::vector<QLine>& GetNetLines() const;
    std::vector<std::pair<QPoint, bool>> GetBookedPoints() const;
#endif // VISUAL_DEBUGGING
};

#endif // AREAMANAGER_H
