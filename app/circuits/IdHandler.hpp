#ifndef IDHANDLER_HPP
#define IDHANDLER_HPP

#include <unordered_set>

#include <QObject>

class IdHandler : public QObject
{
    Q_OBJECT
public:
    explicit IdHandler(QObject *parent = nullptr);
    quint64 NewUid();
    bool InsertUid(quint64 id);
    void RemoveUid(quint64 id);
    bool NewOrderId(quint64 itemType, int orderId);
    void RemoveOrderId(quint64 itemType, int orderId);
    bool ContainsOrderId(quint64 itemType, int orderId) const;
    int GetLastOrderId(quint64 itemType) const;
    void Clear();

private:
    std::unordered_map<quint64, std::unordered_set<int>> m_itemOrderIds;
    std::unordered_map<quint64, int> m_lastItemOrderId;
    std::unordered_set<quint64> m_uids;
    quint64 m_lastUid = 1;
};

#endif // IDHANDLER_HPP
