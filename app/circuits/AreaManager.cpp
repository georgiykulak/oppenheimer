#include "AreaManager.hpp"
#include "graph/DndBackup.hpp"
#include "graph/GraphDesigner.hpp"

#include <QDebug>

AreaManager::AreaManager(QObject *parent)
    : QObject{parent}
    , m_cellSize{16}
{
    m_dndBackup = new DndBackup(parent);
    m_graphDesigner = new GraphDesigner(m_matrix, parent);
}

void AreaManager::SetMatrixSize(QSize windowSize)
{
    const auto height = windowSize.height();
    const auto width = windowSize.width();

    for (int y = 0; y < height; y += m_cellSize)
    {
        std::vector<BookStatus> statusLine;

        for (int x = 0; x < width; x += m_cellSize)
        {
            statusLine.emplace_back(BookStatus::Free);
        }

        m_matrix.emplace_back(std::move(statusLine));
    }

#ifdef VISUAL_DEBUGGING
    SetNetLines(width, height);
#endif // VISUAL_DEBUGGING

    qDebug() << "Matrix size is" << m_matrix.size() << "x"
             << (m_matrix.size() > 0 ? m_matrix[0].size() : 0);
}

bool AreaManager::TryBookArea(
    const QRect& region,
    const std::vector<std::pair<QPoint, QPoint>>& oldNewPoints)
{
    // Closest access point indexes
    IndexRegion idxReg;
    IndexVector intersectedPoints;

    auto handleBookStatus =
        [&intersectedPoints](BookStatus status, IndexPoint point)
        {
            if (status == BookStatus::Line)
                intersectedPoints.push_back(point);

            return status == BookStatus::Item;
        };

    if (!FindSubmatrix(region, idxReg, handleBookStatus))
    {
        // Roll back if the region is booked by other item,
        // so the current item wouldn't be placed upon it
        m_dndBackup->RollBack(m_matrix);
        return false;
    }

    qDebug() << "Found submatrix for booking, region = [{"
             << idxReg.startI << "," << idxReg.startJ << "}, {"
             << idxReg.endI << "," << idxReg.endJ << "}]";

    m_dndBackup->BackupNewPosItem(idxReg, m_matrix);

    // On this stage the are book can be done
    for (std::size_t i = idxReg.startI; i <= idxReg.endI; ++i)
    {
        for (std::size_t j = idxReg.startJ; j <= idxReg.endJ; ++j)
        {
            m_matrix[i][j] = BookStatus::Item;
        }
    }

    std::unordered_map<quint64, Connection> updConnPoints;
    std::unordered_map<quint64, IndexVector> updatedEdges;
    std::unordered_map<quint64, std::pair<QPoint, QPoint>> updatedCurvePoints;

    for (const auto& [oldPoint, newPoint] : oldNewPoints)
    {
        // Roll back if the region is booked by line
        // which can't be redrawn for some reasons
        if (!UpdateConnections(updConnPoints, updatedEdges, updatedCurvePoints, oldPoint, newPoint))
        {
            return false;
        }
    }

    for (const auto& [id, edge] : m_edges)
    {
        if (updatedEdges.contains(id))
        {
            continue;
        }

        for (const auto ip: edge)
        {
            bool flagBreak = false;

            for (const auto isp : intersectedPoints)
            {
                if (ip.i == isp.i && ip.j == isp.j)
                {
                    qDebug() << "Found intersecting edge: -> { start ="
                             << m_connections[id].line.p1() << "end ="
                             << m_connections[id].line.p2() << "}";

                    IndexVector newEdge;
                    QPoint srcCurvePoint;
                    QPoint dstCurvePoint;

                    if (!UpdateConnection(id, newEdge, srcCurvePoint, dstCurvePoint))
                    {
                        return false;
                    }

                    updConnPoints.insert({id, m_connections[id]});
                    updatedEdges.insert({id, newEdge});
                    updatedCurvePoints.insert({id, {srcCurvePoint, dstCurvePoint}});
                    flagBreak = true;
                    break;
                }
            }

            if (flagBreak)
            {
                break;
            }
        }
    }

    for (auto& [id, edge] : updatedEdges)
    {
        m_connections[id] = updConnPoints[id];
        m_edges[id] = std::move(edge);
        m_curvePoints[id] = updatedCurvePoints[id];
    }

    return true;
}

void AreaManager::ClearArea(const QRect &region)
{
    // Closest access point indexes
    IndexRegion idxReg;

    auto handleBookStatus = [](BookStatus, IndexPoint){ return false; };
    FindSubmatrix(region, idxReg, handleBookStatus);

    qDebug() << "Found submatrix for clearing, region = [{"
             << idxReg.startI << "," << idxReg.startJ << "}, {"
             << idxReg.endI << "," << idxReg.endJ << "}]";

    // On this stage the area booking can be done
    for (std::size_t i = idxReg.startI; i <= idxReg.endI; ++i)
        for (std::size_t j = idxReg.startJ; j <= idxReg.endJ; ++j)
            m_matrix[i][j] = BookStatus::Free;
}

void AreaManager::ClearAndBackupArea(const QRect& region)
{
    // Closest access point indexes
    IndexRegion idxReg;

    auto handleBookStatus = [](BookStatus, IndexPoint){ return false; };
    FindSubmatrix(region, idxReg, handleBookStatus);

    qDebug() << "Found submatrix for clearing, region = [{"
             << idxReg.startI << "," << idxReg.startJ << "}, {"
             << idxReg.endI << "," << idxReg.endJ << "}]";

    m_dndBackup->ClearBackup();
    m_dndBackup->BackupOldPosItem(idxReg);

    // On this stage the area booking can be done
    for (std::size_t i = idxReg.startI; i <= idxReg.endI; ++i)
        for (std::size_t j = idxReg.startJ; j <= idxReg.endJ; ++j)
            m_matrix[i][j] = BookStatus::Free;
}

void AreaManager::ClearMatrix()
{
    for (auto& row : m_matrix)
        for (auto& status : row)
            status = BookStatus::Free;
}

std::size_t AreaManager::ConnectionSize() const
{
    return m_edges.size();
}

quint64 AreaManager::AddConnection(const QLine& line,
                                   quint64 startId,
                                   quint64 endId)
{
    static quint64 connId = 1;

    const auto start = line.p1();
    const auto end = line.p2();

    qDebug() << __FUNCTION__ << ": Adding connection { start ="
             << start << "end =" << end << '}';

    IndexVector edge;
    QPoint srcCurvePoint;
    QPoint dstCurvePoint;

    if (!DesignEdge(edge, srcCurvePoint, dstCurvePoint, start, end))
    {
        qWarning() << __FUNCTION__ << ": Can't add connection { start ="
                   << start << "end =" << end << '}';
        return 0;
    }

    qDebug() << __FUNCTION__
             << ": Connection was added successfully { start =" << start
             << "end =" << end << '}';

    m_connections.insert({connId, Connection{startId, endId, line}});
    m_edges.insert({connId, std::move(edge)});
    m_curvePoints.insert({connId, {srcCurvePoint, dstCurvePoint}});

    return connId++;
}

void AreaManager::RemoveConnection(quint64 connId)
{
    if (!m_connections.contains(connId)
     || !m_edges.contains(connId)
     || !m_curvePoints.contains(connId))
    {
        qDebug() << __FUNCTION__ << ": No connection ID '"
                 << connId << "' found";
        return;
    }

    qDebug() << __FUNCTION__ << "Removing connection, connId == "
             << connId;

    // Free earlier booked points for the connection
    const auto& bookedEdge = m_edges[connId];
    for (const auto& idxPoint: bookedEdge)
    {
        qDebug() << "    > {" << idxPoint.i << ',' << idxPoint.j << "}";
        m_matrix[idxPoint.i][idxPoint.j] = BookStatus::Free;
    }

    m_connections.erase(connId);
    m_edges.erase(connId);
    m_curvePoints.erase(connId);
}

std::vector<QPolygon> AreaManager::GetConnectionPolygons() const
{
    std::vector<QPolygon> connections;

    for (const auto& [id, edge] : m_edges)
    {
        QPolygon polygon;

        const auto iter = m_connections.find(id);
        if (iter == m_connections.end())
        {
            continue;
        }

        const auto& line = iter->second.line;
        const auto start = line.p1();
        const auto end = line.p2();

        const auto iter2 = m_curvePoints.find(id);
        if (iter2 == m_curvePoints.end())
        {
            continue;
        }

        const auto& curvePoints = iter2->second;
        const auto srcCurvePoint = curvePoints.first;
        const auto dstCurvePoint = curvePoints.second;

        polygon.append(start);
        polygon.append(srcCurvePoint);

        const int yOffset = srcCurvePoint.y() - edge.at(0).i * m_cellSize;
        std::size_t idx = 0;
        for (; idx < edge.size() - 1; ++idx)
        {
            const auto& ip = edge[idx];
            const auto x = ip.j * m_cellSize;
            const auto y = ip.i * m_cellSize + yOffset;
            polygon.append(QPoint(x, y));
        }

        polygon.append(QPoint(edge.at(idx).j * m_cellSize, dstCurvePoint.y()));
        const auto& ip = edge[idx];
        const int x = ip.j * m_cellSize;
        const int y = dstCurvePoint.y();
        polygon.append(QPoint(x, y));

        polygon.append(dstCurvePoint);
        polygon.append(end);

        connections.emplace_back(polygon);
    }

    return connections;
}

void AreaManager::RemoveAllConnections()
{
    m_connections.clear();
    m_curvePoints.clear();
    m_edges.clear();
}

bool AreaManager::UpdateConnection(quint64 connId,
                                   IndexVector& edge,
                                   QPoint& srcCurvePoint,
                                   QPoint& dstCurvePoint)
{
    const auto iter = m_connections.find(connId);
    if (iter == m_connections.end())
    {
        return false;
    }

    const auto start = iter->second.line.p1();
    const auto end = iter->second.line.p2();

    qDebug() << __FUNCTION__ << ": Updating connection { start ="
             << start << "end =" << end << '}';

    m_dndBackup->BackupConnection(connId, m_edges[connId]);

    auto handleFreeStatus = [](BookStatus& status) {
        status = (status == BookStatus::Item)
                          ? BookStatus::Item
                          : BookStatus::Free;
    };

    FreeConnection(m_edges[connId], handleFreeStatus);

    if (!DesignEdge(edge, srcCurvePoint, dstCurvePoint, start, end))
    {
        qWarning() << __FUNCTION__ << ": Can't add connection { start ="
                   << start << "end =" << end << '}';

        m_dndBackup->RollBack(m_matrix);

        return false;
    }

    qDebug() << __FUNCTION__
             << ": Connection was added successfully { start =" << start
             << "end =" << end << '}';

    return true;
}

bool AreaManager::UpdateConnections(
    std::unordered_map<quint64, Connection>& updConnPoints,
    std::unordered_map<quint64, IndexVector>& updatedEdges,
    std::unordered_map<quint64, std::pair<QPoint, QPoint>>& updatedCurvePoints,
    QPoint oldPoint,
    QPoint newPoint)
{
    qDebug() << __FUNCTION__
             << ": Updating connections with old start/end point =" << oldPoint
             << "to new connection point =" << newPoint;

    if (newPoint.x() < 0 || newPoint.y() < 0)
    {
        m_dndBackup->RollBack(m_matrix);
        return false;
    }

    for (auto&& [id, conn] : m_connections)
    {
        auto line = conn.line;

        if (line.p1() == oldPoint)
        {
            line.setP1(newPoint);
        }
        else if (line.p2() == oldPoint)
        {
            line.setP2(newPoint);
        }
        else
        {
            continue;
        }

        const auto start = line.p1();
        const auto end = line.p2();

        IndexVector newEdge;
        QPoint srcCurvePoint;
        QPoint dstCurvePoint;

        qDebug() << __FUNCTION__<< ": Updating old connection -> { start ="
                 << conn.line.p1() << "end ="
                 << conn.line.p2() << "} with new one -> { start ="
                 << start << "end =" << end << '}';

        m_dndBackup->BackupConnection(id, m_edges[id]);

        auto handleFreeStatus = [](BookStatus& status) {
            status = BookStatus::Free;
        };

        FreeConnection(m_edges[id], handleFreeStatus);

        if (!DesignEdge(newEdge, srcCurvePoint, dstCurvePoint, start, end))
        {
            qWarning() << __FUNCTION__
                       << ": Can't update connection with -> { start =" << start
                       << "end =" << end << '}';

            m_dndBackup->RollBack(m_matrix);

            return false;
        }

        qDebug() << __FUNCTION__<< ": Old connection was updated succesfully with -> {start ="
                 << start << "end =" << end << '}';

        updConnPoints.insert({id, Connection{conn.startId, conn.endId, line}});
        updatedEdges.insert({id, newEdge});
        updatedCurvePoints.insert({id, {srcCurvePoint, dstCurvePoint}});
    }

    return true;
}

void AreaManager::FreeConnection(
    const IndexVector& edge,
    std::function<void (BookStatus&)> handleFreeStatus)
{
    auto iter = edge.begin();

    for (std::size_t i = 0; i < m_matrix.size(); ++i)
    {
        auto& row = m_matrix[i];

        for (std::size_t j = 0; j < row.size(); ++j)
        {
            auto& status = row[j];

            for (auto it = edge.begin(); it != edge.end(); ++it)
            {
                if (iter == edge.end())
                {
                    return;
                }

                if (it->i == i && it->j == j)
                {
                    handleFreeStatus(status);

                    std::next(iter);

                    break;
                }
            }
        }
    }
}

/*
          v startJ         v endJ
startI -> +----------------+
          |(x0, y0)        |
          |                |
          |                |
  endI -> +----------------+
                            (x1, y1)
*/
bool AreaManager::FindSubmatrix(
    const QRect &region,
    IndexRegion& idxReg,
    std::function<bool (BookStatus, IndexPoint)> handleBookStatus)
{
    int x0, y0, x1, y1;
    // Write coordinates
    region.getCoords(&x0, &y0, &x1, &y1);

    // Calculate idxReg.startJ
    auto x0RightOffset = x0 % m_cellSize;
    idxReg.startJ = (x0 - x0RightOffset) / m_cellSize;

    // Calculate idxReg.startI
    auto y0BottomOffset = y0 % m_cellSize;
    idxReg.startI = (y0 - y0BottomOffset) / m_cellSize;

    // Calculate idxReg.endJ
    auto x1RightOffset = x1 % m_cellSize;
    idxReg.endJ = (x1 - x1RightOffset + m_cellSize) / m_cellSize;

    // Calculate idxReg.endI
    auto y1RightOffset = y1 % m_cellSize;
    idxReg.endI = (y1 - y1RightOffset + m_cellSize) / m_cellSize;

    // Book region
    for (std::size_t i = idxReg.startI; i <= idxReg.endI && i < m_matrix.size(); ++i)
    {
        const int y = i * m_cellSize;
        const auto& row = m_matrix[i];

        for (std::size_t j = idxReg.startJ; j <= idxReg.endJ && j < row.size(); ++j)
        {
            const auto status = row[j];
            const int x = j * m_cellSize;

            if (handleBookStatus(status, {i, j}))
            {
                qDebug() << "Can't book that region, the point {x ="
                         << x << ": y =" << y << "} is already booked";
                return false;
            }
        }
    }

    return true;
}

bool AreaManager::CreateClosestStartPoint(IndexVector& edge,
                                          IndexPoint& src,
                                          QPoint& srcCurvePoint,
                                          QPoint start)
{
    if (start.x() > int(m_matrix[0].size() * m_cellSize) - m_cellSize * 2)
        return false;

    int deltaX = m_cellSize * 2 - (start.x() % m_cellSize);
    int curveX = start.x() + deltaX;
    srcCurvePoint = QPoint(curveX, start.y());

    for (std::size_t i = 0; i < m_matrix.size(); ++i)
    {
        auto& row = m_matrix[i];
        int y = i * m_cellSize;

        if (y <= srcCurvePoint.y() && srcCurvePoint.y() < (y + m_cellSize))
        {
            for (std::size_t j = 0; j < row.size(); ++j)
            {
                const int x = j * m_cellSize;

                if (srcCurvePoint.x() <= x && x < (srcCurvePoint.x() + m_cellSize))
                {
                    row[j] = BookStatus::Free;
                    src.i = i;
                    src.j = j;
                    edge.push_back({i, j});
                }
            }

            return true;
        }
    }

    return false;
}

bool AreaManager::CreateClosestEndPoint(IndexVector& edgeEnd,
                                        IndexPoint& dst,
                                        QPoint& dstCurvePoint,
                                        QPoint end)
{
    if (end.x() < m_cellSize * 2)
        return false;

    int deltaX = m_cellSize + (end.x() % m_cellSize);
    int curveX = end.x() - deltaX;
    dstCurvePoint = QPoint(curveX, end.y());

    for (std::size_t i = 0; i < m_matrix.size(); ++i)
    {
        auto& row = m_matrix[i];
        const int y = i * m_cellSize;

        if (y <= dstCurvePoint.y() && dstCurvePoint.y() < (y + m_cellSize))
        {
            for (std::size_t j = 0; j < row.size(); ++j)
            {
                const int x = j * m_cellSize;

                if ((dstCurvePoint.x() - m_cellSize) < x && x <= (dstCurvePoint.x()))
                {
                    row[j] = BookStatus::Free;
                    dst.i = i;
                    dst.j = j;
                    edgeEnd.push_back({i, j});
                }
            }

            return true;
        }
    }

    return false;
}

bool AreaManager::DesignEdge(IndexVector& edge,
                             QPoint& srcCurvePoint,
                             QPoint& dstCurvePoint,
                             QPoint start,
                             QPoint end)
{
    qDebug() << __FUNCTION__ << ": designing edge for vertices -> { start ="
             << start << "end =" << end << '}';

    // Add small line as near as much to first point with same x
    IndexPoint src;
    IndexPoint dst;
    IndexVector edgeEnd;

    if (!CreateClosestStartPoint(edge, src, srcCurvePoint, start)
     || !CreateClosestEndPoint(edgeEnd, dst, dstCurvePoint, end))
    {
        qDebug() << "Item's connection point is to close canvas border";
        return false;
    }

    qDebug() << __FUNCTION__ << ": index points for path searching -> [ src = {"
             << src.i << ',' << src.j << "}, dst = {"
             << dst.i << ',' << dst.j << "} ]";

    if (!m_graphDesigner->Design(edge, src, dst))
    {
        qWarning() << "A* Search agorithm didn't succeed";
        return false;
    }

    edge.insert(edge.end(), edgeEnd.begin(), edgeEnd.end());

    return true;
}

#ifdef VISUAL_DEBUGGING

const std::vector<QLine>& AreaManager::GetNetLines() const
{
    return m_net;
}

std::vector<std::pair<QPoint, bool>> AreaManager::GetBookedPoints() const
{
    std::vector<std::pair<QPoint, bool>> points;

    for (std::size_t i = 0; i < m_matrix.size(); ++i)
    {
        const auto& row = m_matrix[i];

        for (std::size_t j = 0; j < row.size(); ++j)
        {
            const auto status = row[j];

            if (status != BookStatus::Free)
            {
                const auto x = j * m_cellSize;
                const auto y = i * m_cellSize;
                points.emplace_back(QPoint(x, y), status == BookStatus::Item);
            }
        }
    }

    return points;
}

void AreaManager::SetNetLines(int width, int height)
{
    m_net.clear();

    for (std::size_t i = 0; i < m_matrix.size(); ++i)
    {
        const int y = i * m_cellSize;
        QLine yLine(QPoint(0, y), QPoint(width - 1, y));
        m_net.emplace_back(std::move(yLine));
    }

    if (m_matrix.size() > 0)
    {
        const auto& row = m_matrix.at(0);

        for (std::size_t j = 0; j < row.size(); ++j)
        {
            const int x = j * m_cellSize;
            QLine xLine(QPoint(x, 0), QPoint(x, height - 1));
            m_net.emplace_back(std::move(xLine));
        }
    }
}

#endif // VISUAL_DEBUGGING
