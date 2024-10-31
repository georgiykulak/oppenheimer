#include "GraphDesigner.hpp"
#include <Config.hpp>

#include <stack>
#include <cmath>

#include <QDebug>

GraphDesigner::GraphDesigner(StatusMatrix& matrix, QObject *parent)
    : QObject{parent}
    , m_matrix{matrix}
{

}

bool GraphDesigner::Design(IndexVector &edge, const IndexPoint src, const IndexPoint dst)
{
    return AStarSearch(edge, src, dst);
}

// A Utility Function to check whether
// given cell (row, col) is a valid cell or not
bool GraphDesigner::IsValid(const std::size_t row,
                            const std::size_t col) const
{
    if (m_matrix.empty())
        return false;

    const auto rowSize = m_matrix.size();
    const auto colSize = m_matrix.at(0).size();

    if (colSize == 0)
        return false;

    return row < rowSize && col < colSize;
}
// A Utility Function to check whether
// the given cell is blocked or not
bool GraphDesigner::IsUnBlocked(const std::size_t row,
                                const std::size_t col) const
{
    return m_matrix[row][col] != BookStatus::Item;
}

// A Utility Function to check whether
// destination cell has been reached or not
bool GraphDesigner::isDestination(const std::size_t row,
                                  const std::size_t col,
                                  const IndexPoint dst) noexcept
{
    return row == dst.i && col == dst.j;
}

// A Utility Function to calculate the 'h' heuristics.
double GraphDesigner::calculateHValue(const std::size_t row,
                                      const std::size_t col,
                                      IndexPoint dst)
{
#ifdef EUCLIDEAN_DISTANCE_FORMULA
    // Return using the Euclidean distance formula
    return (double)sqrt(std::pow(((long long)row - (long long)dst.i), 2)
                      + std::pow(((long long)col - (long long)dst.j), 2));
#else
    // Return using the Manhattan distance formula
    return double(std::abs((long long)row - (long long)dst.i) +
                  std::abs((long long)col - (long long)dst.j));
#endif // EUCLIDEAN_DISTANCE_FORMULA
}

// A Utility Function to trace the path from the source to destination
void GraphDesigner::tracePath(IndexVector& edge,
                              IndexPoint dst,
                              const CellMatrix& cellDetails)
{
    qDebug() << "The Path is:";

    std::size_t row = dst.i;
    std::size_t col = dst.j;

    std::stack<IndexPoint> path;

    while (
        !(cellDetails[row][col].parent_i == row &&
          cellDetails[row][col].parent_j == col)
        )
    {
        path.push({row, col});
        int temp_row = cellDetails[row][col].parent_i;
        int temp_col = cellDetails[row][col].parent_j;
        row = temp_row;
        col = temp_col;
    }

    path.push({row, col});
    IndexVector newEdge;

    while (!path.empty())
    {
        const auto point = path.top();
        path.pop();

        m_matrix[point.i][point.j] = BookStatus::Line;

        qDebug() << "    > {" << point.i << ',' << point.j << "}";

        newEdge.push_back(point);
    }

    edge.insert(edge.end(), newEdge.begin(), newEdge.end());
}

void GraphDesigner::fillCellMatrix(CellMatrix& cellDetails)
{
    for (const auto& _ : m_matrix)
    {
        std::vector<Cell> cells;

        for (const auto __ : _)
        {
            (void)_;
            (void)__;

            Cell cell;
            cell.f = std::numeric_limits<double>::max();
            cell.g = std::numeric_limits<double>::max();
            cell.h = std::numeric_limits<double>::max();
            cell.parent_i = std::numeric_limits<std::size_t>::max();
            cell.parent_j = std::numeric_limits<std::size_t>::max();

            cells.emplace_back(std::move(cell));
        }

        cellDetails.emplace_back(std::move(cells));
    }
}

// A function to find the shortest path
// between a given start point to an end point
// according to "A* Search Algorithm"
bool GraphDesigner::AStarSearch(IndexVector& edge,
                                const IndexPoint src,
                                const IndexPoint dst)
{
    if (!IsValid(src.i, src.j))
    {
        qWarning() << __FUNCTION__ << ": Source point ("
                   << src.i << ',' << src.j << ") is out of range";
        return false;
    }

    if (!IsValid(dst.i, dst.j))
    {
        qWarning() << __FUNCTION__ << ": Destination point ("
                   << dst.i << ',' << dst.j << ") is out of range";
        return false;
    }

    if (!IsUnBlocked(src.i, src.j))
    {
        qWarning() << __FUNCTION__ << ": Source point ("
                   << src.i << ',' << src.j << ") is blocked";
        return false;
    }

    if (!IsUnBlocked(dst.i, dst.j))
    {
        qWarning() << __FUNCTION__ << ": Destination point ("
                   << dst.i << ',' << dst.j << ") is blocked";
        return false;
    }

    if (isDestination(src.i, src.j, dst))
    {
        qWarning() << __FUNCTION__
                   << ": Source and destination points are equal "
                   << '(' << src.i << ',' << src.j << ')';
        return false;
    }

    // Declare a matrix of cells to hold the details
    CellMatrix cellDetails;

    fillCellMatrix(cellDetails);

    // Initialising the parameters of the starting node
    std::size_t i = src.i;
    std::size_t j = src.j;
    cellDetails[i][j].f = 0.0;
    cellDetails[i][j].g = 0.0;
    cellDetails[i][j].h = 0.0;
    cellDetails[i][j].parent_i = i;
    cellDetails[i][j].parent_j = j;

    /*
     Create an open list having information as-
     <f, <i, j>>
     where f = g + h,
     and i, j are the row and column index of that cell
     Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
     This open list is implemented as a set of pair of
     pair.
    */
    std::set<FCell> openList;

    // Put the starting cell on the open list and set its
    // 'f' as 0
    openList.insert(makeFCell(0.0, src));

    qDebug() << __FUNCTION__ << ": Search has been started";

    while (!openList.empty())
    {
        FCell p = *openList.begin();

        // Remove this vertex from the open list
        openList.erase(openList.begin());

        // Add this vertex to the closed list
        i = p.second.i;
        j = p.second.j;

        // To store the 'g', 'h' and 'f' of the 4 successors
        double gNew, hNew, fNew;
        bool foundDst;

        /*
         Generating all the 4 successor of this cell

                    N
                    |
                    |
             W----Point----E
                    |
                    |
                    S

         Selected point --> (i, j)
         N - North      --> (i-1, j)
         S - South      --> (i+1, j)
         E - East       --> (i, j+1)
         W - West       --> (i, j-1)
        */

        // ---------- 1st Successor (North) ---------- //
        foundDst = processSuccessor({i - 1, j},
                                    i, j, gNew, hNew, fNew,
                                    dst, edge, openList, cellDetails);

        if (foundDst)
            return true;

        // ---------- 2nd Successor (South) ---------- //
        foundDst = processSuccessor({i + 1, j},
                                    i, j, gNew, hNew, fNew,
                                    dst, edge, openList, cellDetails);

        if (foundDst)
            return true;

        // ---------- 3rd Successor (East) ----------- //
        foundDst = processSuccessor({i, j + 1},
                                    i, j, gNew, hNew, fNew,
                                    dst, edge, openList, cellDetails);

        if (foundDst)
            return true;

        // ---------- 4th Successor (West) ----------- //
        foundDst = processSuccessor({i, j - 1},
                                    i, j, gNew, hNew, fNew,
                                    dst, edge, openList, cellDetails);

        if (foundDst)
            return true;
    }

    // When the destination cell is not found and the open
    // list is empty, then we conclude that we failed to
    // reach the destination cell. This may happen when the
    // there is no way to destination cell (due to
    // blockages)

    qDebug() << __FUNCTION__ << ": Search has been finished unsuccesfully";

    return false;
}

bool GraphDesigner::processSuccessor(const IndexPoint qtrIdx,
                                     const std::size_t i,
                                     const std::size_t j,
                                     double& gNew,
                                     double& hNew,
                                     double& fNew,
                                     IndexPoint dst,
                                     IndexVector& edge,
                                     std::set<FCell>& openList,
                                     CellMatrix& cellDetails)
{
    // Only process this cell if this is a valid one
    if (!IsValid(qtrIdx.i, qtrIdx.j))
        return false;

    // If the destination cell is the same as the
    // current successor
    if (isDestination(qtrIdx.i, qtrIdx.j, dst))
    {
        // Set the Parent of the destination cell
        cellDetails[qtrIdx.i][qtrIdx.j].parent_i = i;
        cellDetails[qtrIdx.i][qtrIdx.j].parent_j = j;

        qDebug() << "The destination cell is found";

        tracePath(edge, dst, cellDetails);
        return true;
    }

    // If the successor is already on the closed
    // list or if it is blocked, then ignore it.
    // Else do the following
    else if (IsUnBlocked(qtrIdx.i, qtrIdx.j))
    {
        gNew = cellDetails[i][j].g + 1.0;
        hNew = calculateHValue(qtrIdx.i, qtrIdx.j, dst);
        fNew = gNew + hNew;

        // If it isn’t on the open list, add it to
        // the open list. Make the current square
        // the parent of this square. Record the
        // f, g, and h costs of the square cell
        //                OR
        // If it is on the open list already, check
        // to see if this path to that square is
        // better, using 'f' cost as the measure.
        if (cellDetails[qtrIdx.i][qtrIdx.j].f == std::numeric_limits<double>::max()
         || cellDetails[qtrIdx.i][qtrIdx.j].f > fNew)
        {
            openList.insert(
                makeFCell(fNew, {qtrIdx.i, qtrIdx.j})
            );

            // Update the details of this cell
            cellDetails[qtrIdx.i][qtrIdx.j].f = fNew;
            cellDetails[qtrIdx.i][qtrIdx.j].g = gNew;
            cellDetails[qtrIdx.i][qtrIdx.j].h = hNew;
            cellDetails[qtrIdx.i][qtrIdx.j].parent_i = i;
            cellDetails[qtrIdx.i][qtrIdx.j].parent_j = j;
        }
    }

    return false;
}
