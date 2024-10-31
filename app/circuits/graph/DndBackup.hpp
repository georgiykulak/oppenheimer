#ifndef DNDBACKUP_HPP
#define DNDBACKUP_HPP

#include "CommonTypes.hpp"

class DndBackup : public QObject
{
    Q_OBJECT
public:
    explicit DndBackup(QObject* parent = nullptr);

    void BackupOldPosItem(const IndexRegion& idxReg);
    void BackupNewPosItem(const IndexRegion& idxReg,
                          const StatusMatrix& matrix);
    void BackupConnection(quint64 connId,
                          const IndexVector& edge);
    void RollBack(StatusMatrix& matrix);
    void ClearBackup();

private:
    IndexRegion m_oldSubMatrix;
    IndexRegion m_newSubMatrix;
    std::vector<BookStatus> m_newSubMatrixStatuses;
    std::unordered_map<std::size_t, IndexVector> m_edges;
};

#endif // DNDBACKUP_HPP
