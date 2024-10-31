#include "DndBackup.hpp"

DndBackup::DndBackup(QObject* parent)
    : QObject{parent}
{}

void DndBackup::BackupOldPosItem(const IndexRegion& idxReg)
{
    m_oldSubMatrix = idxReg;
}

void DndBackup::BackupNewPosItem(
    const IndexRegion &idxReg,
    const std::vector<std::vector<BookStatus> >& matrix)
{
    if (matrix.empty())
        return;

    const auto rowSize = matrix.size();
    const auto colSize = matrix.at(0).size();
    m_newSubMatrix = idxReg;

    for (std::size_t i = m_newSubMatrix.startI; i <= m_newSubMatrix.endI && i < rowSize; ++i)
        for (std::size_t j = m_newSubMatrix.startJ; j <= m_newSubMatrix.endJ && j < colSize; ++j)
            m_newSubMatrixStatuses.push_back(matrix[i][j]);
}

void DndBackup::BackupConnection(quint64 connId,
                                 const IndexVector& edge)
{
    m_edges.insert({connId, edge});
}

void DndBackup::RollBack(StatusMatrix& matrix)
{
    if (matrix.empty())
        return;

    const auto rowSize = matrix.size();
    const auto colSize = matrix.at(0).size();

    for (const auto& [_, edge] : m_edges)
        for (const auto ip : edge)
            matrix[ip.i][ip.j] = BookStatus::Line;

    for (std::size_t i = m_oldSubMatrix.startI; i <= m_oldSubMatrix.endI && i < rowSize; ++i)
        for (std::size_t j = m_oldSubMatrix.startJ; j <= m_oldSubMatrix.endJ && j < colSize; ++j)
            matrix[i][j] = BookStatus::Item;

    std::size_t newI = 0;
    for (std::size_t i = m_newSubMatrix.startI; i <= m_newSubMatrix.endI && i < rowSize; ++i)
        for (std::size_t j = m_newSubMatrix.startJ; j <= m_newSubMatrix.endJ && j < colSize; ++j)
            matrix[i][j] = m_newSubMatrixStatuses[newI++];
}

void DndBackup::ClearBackup()
{
    m_oldSubMatrix = {};
    m_newSubMatrix = {};
    m_newSubMatrixStatuses.clear();
    m_edges.clear();
}

