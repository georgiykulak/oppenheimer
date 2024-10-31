#ifndef GRAPHDESIGNER_HPP
#define GRAPHDESIGNER_HPP

#include "CommonTypes.hpp"

#include <set>

class GraphDesigner : public QObject
{
    Q_OBJECT
public:
    using FCell = std::pair<double, IndexPoint>;

    // A structure to hold the necessary parameters
    struct Cell
    {
        // Row and Column index of its parent. Note that:
        // 0 <= i <= row_size-1 && 0 <= j <= column_size-1
        std::size_t parent_i;
        std::size_t parent_j;
        // f = g + h
        double f;
        double g;
        double h;
    };

    using CellMatrix = std::vector<std::vector<Cell>>;

    static FCell makeFCell(double f, IndexPoint p) { return std::make_pair(f, p); }

    explicit GraphDesigner(StatusMatrix& matrix, QObject *parent = nullptr);

    bool Design(IndexVector& edge,
                const IndexPoint src,
                const IndexPoint dst);

private:
    StatusMatrix& m_matrix;

    bool IsValid(const std::size_t row,
                 const std::size_t col) const;
    bool IsUnBlocked(const std::size_t row,
                     const std::size_t col) const;
    bool isDestination(const std::size_t row,
                       const std::size_t col,
                       const IndexPoint dst) noexcept;
    double calculateHValue(const std::size_t row,
                           const std::size_t col,
                           IndexPoint dst);
    void tracePath(IndexVector& edge,
                   IndexPoint dst,
                   const CellMatrix& cellDetails);
    void fillCellMatrix(CellMatrix& cellDetails);
    bool AStarSearch(IndexVector& edge,
                     const IndexPoint src,
                     const IndexPoint dst);
    bool processSuccessor(const IndexPoint qtrIdx,
                          const std::size_t i,
                          const std::size_t j,
                          double& gNew,
                          double& hNew,
                          double& fNew,
                          IndexPoint dst,
                          IndexVector& edge,
                          std::set<FCell>& openList,
                          CellMatrix& cellDetails);
};

#endif // GRAPHDESIGNER_HPP
